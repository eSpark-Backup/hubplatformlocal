local self, device_id, node_id, should_reset_configuration = ...

local zwave = require( "zwave" )
local core = require("core")
local class_ids = require("HUB:zwave/scripts/resources/class_ids")

local cc_configuration = zwave.get_command_class(node_id, 0, class_ids.configuration)

local function reset_each_setting(specific_settings_only)
    local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
    local setting_ids = core.get_setting_ids_by_device_id(device_id)
    if setting_ids then
        for _, setting_id in ipairs(setting_ids) do
            local descriptor = storage:call("find_setting_descriptor", setting_id)
            if descriptor.class_id ~= class_ids.configuration then
                self:call("reset_specific_setting", setting_id, descriptor)
            elseif not specific_settings_only then
                self:call("reset_configuration_setting", setting_id, descriptor.node_id, should_reset_configuration)
            end
        end
    end
end

if cc_configuration and cc_configuration.version >= 4 then
    zwave.reset_all_configuration_values(node_id)

    reset_each_setting(true)
    
    local node_info = zwave.get_node_info(node_id)
    
    local dev_handler_id = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/create_device_type_id")(
        node_info.manufacturer_id,
        node_info.product_type,
        node_info.product_id
    )
    
    local device_info = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_info")(dev_handler_id)

    zwave.request_node_values(
        node_id, 
        device_info.request_value_delay,
        hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_unset_pin_codes_count_to_stop_requesting")(device_info) 
    )
else
    reset_each_setting()
end
