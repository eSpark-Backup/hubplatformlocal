local zwave_node, items_old_names, storage_item_deleter, storage_setting_deleter, items_comparator, items_to_change_value = ...

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

local device_id = storage:call("find_device_id", zwave_node.id)

if not device_id then
    hubplugins.weak_loadfile("HUB:zwave/scripts/events/events_handling")({
        event = "node_added",
        node_id = zwave_node.id
    })
    return
end


local core = require("core")

local dev_nodes = {}

storage:call("delete_table", storage:call("get_monitoring_handler_key", zwave_node.id))

local dev_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_handler_inclusion")(zwave_node)
local new_tree_model = dev_handler:call("process_node", zwave_node).device_tree_model
new_tree_model:call("direct_walk",
    function (dev_node)
        local dev_data = new_tree_model:call("get_data", dev_node)
        dev_data.merged_items_map = dev_handler:call("get_merged_items", dev_data.items)
        dev_data.items = {}
        for _, merged_item_info in pairs(dev_data.merged_items_map) do
            table.insert(dev_data.items, merged_item_info.main_item)
        end
        table.insert(dev_nodes, dev_node)
    end
)

local core_devices = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_core_devices")(device_id)

local calculateOverlapFactor = hubplugins.weak_loadfile("HUB:zwave/scripts/migration/utils/calculate_overlap_factor")

local matrix = hubplugins.weak_loadfile("HUB:zwave/scripts/migration/utils/create_matrix")()

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
local devices_to_delete = {}

local function compare_items(old_item, new_item)
    return items_comparator(storage, old_item, new_item, items_old_names)
end

local function migrate_items(old_items, new_items)
    for _, old_item in ipairs(old_items) do
        local found = false
        for _, new_item in ipairs(new_items) do
            if compare_items(old_item, new_item) then
                new_item.id = old_item.id

                if new_item.value_type == old_item.value_type and not items_to_change_value[new_item.name] then
                    new_item.value = old_item.value
                end
                found = true
                break
            end
        end
        if not found then
            table.insert(items_to_delete, old_item)
        end
    end
end

local function migrate_core_dev_to_dev_node_data(dev_core, dev_node_data, override_name)
    dev_node_data.info.id = dev_core.id
    if not override_name then
        dev_node_data.info.name = dev_core.name
    end

    local items = core.get_items_by_device_id(dev_core.id)
    if items then
        migrate_items(items, dev_node_data.items)
    end
end


local migrated_core_devices = {}

local config = hubplugins.weak_require("HUB:zwave/scripts/migration/config")

for i = 1, #values do
    local val = values[i]
    if val then
        local device = core_devices[val.x]
        local dev_node = dev_nodes[val.y]

        local is_correct_factor = val.factor >= config.overlap_factor
        local is_incomplete_device = hubplugins.weak_loadfile("HUB:zwave/scripts/migration/utils/is_incomplete_device")(device)
        local should_merge_root_to_root = is_incomplete_device and device.parent_device_id == "" and not dev_node.parent

        if is_correct_factor or should_merge_root_to_root then
            migrate_core_dev_to_dev_node_data(
                device,
                new_tree_model:call("get_data", dev_node),
                is_incomplete_device
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

local function add_setting(setting, device_id)
    local setting_id = core.add_setting(setting)

    storage:call("insert_setting_mapping", setting_id, setting.descriptor)
    storage:call("insert_enum_value_mapping", setting_id, setting.value_mapping)
end

local function remove_setting(setting_id)
    core.remove_setting(setting_id)
    storage_setting_deleter(setting_id)
end

local function find_setting_id(descriptor)
    return storage:call("find_setting_id", descriptor.node_id, descriptor.class_id, descriptor.tag)
end

local function list_to_set(list)
    local set = {}
    for _, item in pairs(list) do
        set[item] = true
    end
    return set
end

local function update_settings(dev_info, new_settings)
    local old_setting_ids = list_to_set(core.get_setting_ids_by_device_id(device_id) or {})

    -- Add settings that didn't exist before
    for _, setting in ipairs(new_settings) do
        setting.device_id = dev_info.id

        local old_setting_id = find_setting_id(setting.descriptor)
        if old_setting_id ~= nil then
            old_setting_ids[old_setting_id] = nil
            core.modify_setting(old_setting_id, setting)
        else
            add_setting(setting, dev_info.id)
        end
    end

    -- Remove settings that no longer exist
    for setting_id, _ in pairs(old_setting_ids) do
        remove_setting(setting_id)
    end
end


local callback = function(dev_node)
    local dev_data = new_tree_model:call("get_data", dev_node)
    local parent_dev_node = new_tree_model:call("get_parent", dev_node)

    if not parent_dev_node then
        dev_data.info.id = ensure_device(dev_data.info)
        storage:call("remove_device_mapping", zwave_node.id)
        storage:call("insert_device_mapping", dev_data.info.id, zwave_node.id)

        if hubplugins.device_settings_enabled then
            update_settings(dev_data.info, dev_data.settings)
        end
    else
        local parent_dev_data = new_tree_model:call("get_data", parent_dev_node)
        dev_data.info.parent_device_id = parent_dev_data.info.id
        dev_data.info.id = ensure_device(dev_data.info)
    end

    for _, merged_item_info in pairs(dev_data.merged_items_map) do
        local item = merged_item_info.main_item
        item.device_id = dev_data.info.id

        local item_id = item.id
        if item_id then
            storage_item_deleter(item, true)

            core.modify_item(item_id, item)
        else
            item_id = core.add_item(item)
        end

        storage:call("insert_item_mapping", item_id, merged_item_info.descriptors)
    end
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

for _, dev in ipairs(devices_to_delete) do
    local items = core.get_items_by_device_id(dev.id)
    if items then
        for _, item in ipairs(items) do
            remove_item(item)
        end
    end
    print(("[migration] remove device. id: %s, name: %s"):format(dev.id, dev.name))
    core.remove_device_sync(dev.id)
end
