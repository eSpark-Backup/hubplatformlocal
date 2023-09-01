local self, inclusion_context = ...

local zwave = assert( require( "zwave" ), "Fail to load the 'zwave' functionality" )
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local device_tree_model = inclusion_context.device_tree_model
local zwave_node_id = inclusion_context.zwave_node.id
local channels_count = #inclusion_context.zwave_node.channels
inclusion_context.zwave_node = nil
hubplugins.try_collectgarbage()

local callback = function(dev_node)
    hubplugins.try_collectgarbage()
    
    local core = assert(require("core"), "Fail to load the 'core' functionality")

    local dev_data = device_tree_model:call("get_data", dev_node)

    local parent_dev_node = device_tree_model:call("get_parent", dev_node)

    if not parent_dev_node then
        local zwave_node_id_int = tonumber(zwave_node_id)
        local smart_start = zwave.get_node_info(zwave_node_id_int).is_smart_include and
                hubplugins.object( "smart_start", "HUB:zwave/scripts/utils" )

        if smart_start then
            smart_start:call( "setup_device_info", dev_data.info, zwave_node_id_int )
        end

        if not dev_data.info.info then
            dev_data.info.info = {}
        end
        dev_data.info.info[ "channels.count" ] = tostring(channels_count)

        dev_data.info.id = assert(core.add_device(dev_data.info), "Adding device to 'core' module failed")
        storage:call("insert_device_mapping", dev_data.info.id, zwave_node_id)

        if smart_start then
            smart_start:call( "start_tracking_device_changes", dev_data.info.id )
        end
    else


        local parent_dev_data = device_tree_model:call("get_data", parent_dev_node)
        dev_data.info.parent_device_id = parent_dev_data.info.id
        dev_data.info.id = core.add_device(dev_data.info)
    end

    local merged_items_map = self:call("get_merged_items", dev_data.items)

    dev_data.items = nil
    hubplugins.try_collectgarbage()

    local dev_data_info_id = dev_data.info.id

    for _, merged_item_info in ipairs(merged_items_map) do
        merged_item_info.main_item.device_id = dev_data_info_id

        local item_id = core.add_item(merged_item_info.main_item)

        storage:call("insert_item_mapping", item_id, merged_item_info.descriptors)

        merged_item_info.descriptors = nil
        merged_item_info.main_item = nil
        hubplugins.try_collectgarbage()
    end

    merged_items_map = nil
    hubplugins.try_collectgarbage()

    for _, setting in ipairs(dev_data.settings) do
        setting.device_id = dev_data_info_id

        local setting_id = core.add_setting(setting)

        storage:call("insert_setting_mapping", setting_id, setting.descriptor)
        storage:call("insert_enum_value_mapping", setting_id, setting.value_mapping)
    end

    dev_data.settings = nil
    hubplugins.try_collectgarbage()
end

device_tree_model:call("direct_walk", callback)
