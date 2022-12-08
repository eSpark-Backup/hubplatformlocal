local dev_info = ...

local core = assert(require "core", "Fail to load core functionality")
local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")

local handler = {}

local function remove_device_content(device_id)
    local storage_helper = hubplugins.object("storage_helper", "HUB:zigbee/scripts/utils")

    local function callback(device)
        local items = core.get_items_by_device_id(device.id)
        if items then
            for _, item in ipairs(items) do
                storage_helper:call("remove_item_mapping", item.id)
                if item.name == item_names.user_codes then
                    storage_helper:call("remove_value", storage_helper:call("get_usernames_tablename", item.id))
                end
            end
        else
            print("[exclusion.remove_node]: items not found for device " .. device.id);
        end

        local settings_ids = core.get_setting_ids_by_device_id(device.id)
        if settings_ids then
            for _, setting_id in ipairs(settings_ids) do
                storage_helper:call("remove_setting_mapping", setting_id)
            end
        end
    end

    hubplugins.weak_loadfile("HUB:zigbee/scripts/helpers/for_each_device")(device_id, callback, core)
end

function handler.remove_node(self, node_id)
    local storage_helper = hubplugins.object("storage_helper", "HUB:zigbee/scripts/utils")

    local device_id = storage_helper:call("find_device_id", node_id)

    if not device_id then return end

    remove_device_content(device_id)

    print( "LUA: function handler.remove_node: _id - " .. tostring(node_id) .. ", value - " .. device_id )
    storage_helper:call("remove_device_mapping", node_id)
    core.remove_device(device_id)
end

function handler.remove_device(self, device_id)
    remove_device_content(device_id)
    core.remove_device(device_id)
end

return handler
