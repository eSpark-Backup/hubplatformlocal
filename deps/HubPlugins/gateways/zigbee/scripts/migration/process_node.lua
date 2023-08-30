local zigbee_node, items_old_names, storage_item_deleter, should_use_old_item_value = ...

local storage = hubplugins.object("storage_helper", "HUB:zigbee/scripts/utils")

local device_id = storage:call("find_device_id", zigbee_node.id)

if not device_id then
    hubplugins.weak_loadfile("HUB:zigbee/scripts/events/zigbee/events_handling")({
        event = "node_added",
        node_id = zigbee_node.id
    })
    return
end


local core = require("core")

local dev_nodes = {}

local dev_handler = hubplugins.weak_loadfile("HUB:zigbee/scripts/helpers/get_device_handler_inclusion")(zigbee_node)
local new_tree_model = dev_handler:build_new_tree_model(zigbee_node).tree_model
new_tree_model:call("direct_walk",
    function (dev_node)
        local dev_data = new_tree_model:call("get_data", dev_node)
        dev_data.merged_items_map = dev_handler:get_merged_items(dev_data.items)
        dev_data.items = {}
        for _, merged_item_info in pairs(dev_data.merged_items_map) do
            table.insert(dev_data.items, merged_item_info.main_item)
        end
        table.insert(dev_nodes, dev_node)
    end
)

local core_devices = hubplugins.weak_loadfile("HUB:zigbee/scripts/helpers/get_core_devices")(device_id)

local calculateOverlapFactor = hubplugins.weak_loadfile("HUB:zigbee/scripts/migration/utils/calculate_overlap_factor")

local matrix = hubplugins.weak_loadfile("HUB:zigbee/scripts/migration/utils/create_matrix")()

local values = {}
for x = 1, #core_devices do
    for y = 1, #dev_nodes do
        local val = matrix[x][y]
        val.x = x
        val.y = y
        val.factor = calculateOverlapFactor(core_devices[x], new_tree_model:call("get_data", dev_nodes[y]), items_old_names)
        table.insert(values, val)
    end
end

table.sort(values, function (lhs, rhs) return lhs.factor > rhs.factor end)

for idx, val in ipairs(values) do
    val.sorted_index = idx
end

local items_to_delete = {}
local settings_to_delete = {}
local devices_to_delete = {}

local function migrate_items(old_items, new_items)
    for _, old_item in ipairs(old_items) do
        local found = false
        for _, new_item in ipairs(new_items) do
            if (new_item.name == old_item.name) or (new_item.name == items_old_names[old_item.name]) then
                new_item.id = old_item.id
                found = true
                break
            end
        end
        if not found then
            table.insert(items_to_delete, old_item)
        end
    end
end

local function migrate_core_dev_to_dev_node_data(dev_core, dev_node_data)
    dev_node_data.info.id = dev_core.id
    dev_node_data.info.name = dev_core.name

    local items = core.get_items_by_device_id(dev_core.id)
    if items ~= nil then
        migrate_items(items, dev_node_data.items)
    end
end


local migrated_core_devices = {}

local config = hubplugins.weak_loadfile("HUB:zigbee/scripts/migration/config")()

for i = 1, #values do
    local val = values[i]
    if val then
        if val.factor < config.overlap_factor then
            break
        end

        migrate_core_dev_to_dev_node_data(
            core_devices[val.x],
            new_tree_model:call("get_data", dev_nodes[val.y])
        )
        migrated_core_devices[val.x] = true

        for x, _ in ipairs(core_devices) do
            values[ matrix[x][val.y].sorted_index ] = nil
        end

        for y, _ in ipairs(dev_nodes) do
            values[ matrix[val.x][y].sorted_index ] = nil
        end
    end
end

for x, dev in ipairs(core_devices) do
    if not migrated_core_devices[x] then
        table.insert(devices_to_delete, dev)
    end
end

local function ensure_device(dev_info)
    local dev_id = dev_info.id
    if dev_id then
        core.modify_device(dev_id, dev_info)
        core.update_device_status(dev_id, "idle")
        print(("[migration] modified device. id: %s, name: %s"):format(dev_id, dev_info.name))
    else
        dev_id = core.add_device(dev_info)
        print(("[migration] added new device. id: %s, name: %s"):format(dev_id, dev_info.name) )
    end

    return dev_id
end

local function list_to_set(list)
    local set = {}
    for _, item in pairs(list) do
        set[item] = true
    end
    return set
end

local function update_items(dev_data)
    for _, merged_item_info in pairs(dev_data.merged_items_map) do
        local item = merged_item_info.main_item
        item.device_id = dev_data.info.id

        local item_id = item.id
        if item_id then
            storage_item_deleter(item)

            if should_use_old_item_value(item, merged_item_info.descriptors) then
                item.value = core.get_item(item_id).value
            end

            core.modify_item(item_id, item)
        else
            item_id = core.add_item(item)
        end

        storage:call("insert_item_mapping", item_id, merged_item_info.descriptors)
    end
end

local function update_settings(dev_data)
    local old_setting_ids = list_to_set(core.get_setting_ids_by_device_id(device_id) or {})

    -- Add settings that didn't exist before
    for _, setting in ipairs(dev_data.settings) do
        setting.device_id = dev_data.info.id

        local old_setting_id = storage:call("find_setting_id", setting.descriptor)
        if old_setting_id ~= nil then
            old_setting_ids[old_setting_id] = nil
            core.modify_setting(old_setting_id, setting)
        else
            local setting_id = core.add_setting(setting)
            storage:call("insert_setting_mapping", setting_id, setting.descriptor)
        end
    end

    -- Remove settings that no longer exist
    for setting_id, _ in pairs(old_setting_ids) do
        storage:call("remove_setting_mapping", setting_id)
        core.remove_setting(setting_id)
    end
end

local callback = function(dev_node)
    local dev_data = new_tree_model:call("get_data", dev_node)
    local parent_dev_node = new_tree_model:call("get_parent", dev_node)

    if not parent_dev_node then
        dev_data.info.id = ensure_device(dev_data.info)
        storage:call("remove_device_mapping", zigbee_node.id)
        storage:call("insert_device_mapping", dev_data.info.id, zigbee_node.id)
    else
        local parent_dev_data = new_tree_model:call("get_data", parent_dev_node)
        dev_data.info.parent_device_id = parent_dev_data.info.id
        dev_data.info.id = ensure_device(dev_data.info)
    end

    update_items(dev_data)

    update_settings(dev_data)
end

new_tree_model:call("direct_walk", callback)


local function remove_item(item)
    print(("[migration] remove item. id: %s, name: %s"):format(item.id, item.name))
    storage_item_deleter(item)
    core.remove_item(item.id)
end

for _, item in ipairs(items_to_delete) do
    remove_item(item)
end

local function remove_setting(setting_id)
    storage:call("remove_setting_mapping", setting_id)
    core.remove_setting(setting_id)
end

for _, setting_id in ipairs(settings_to_delete) do
    remove_setting(setting_id)
end

for _, dev in ipairs(devices_to_delete) do
    local items = core.get_items_by_device_id(dev.id)
    for _, item in ipairs(items) do
        remove_item(item)
    end
    print(("[migration] remove device. id: %s, name: %s"):format(dev.id, dev.name))
    core.remove_device_sync(dev.id)
end

dev_handler:configure_attributes_reporting(new_tree_model)
