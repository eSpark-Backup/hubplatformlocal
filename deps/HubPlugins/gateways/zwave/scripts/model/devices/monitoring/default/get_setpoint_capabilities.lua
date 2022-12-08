local self, cc_setpoint, setpoint_type, target_item_name, item_id = ...

local temperature_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/temperature")

local function check_min_max(setpoint)
    if ( not setpoint ) or ( not setpoint.type ) then
        return
    end
    
    -- Check capabilities format
    if setpoint.min_value and setpoint.max_value and setpoint.min_scale and setpoint.max_scale then
        if temperature_enum.value_to_token[setpoint.min_scale] ~= nil and
           temperature_enum.value_to_token[setpoint.max_scale] ~= nil then
            return
        end
    end
    
    local core = require("core")
    local exist_item = core.get_item(item_id)
    local dev_type_id

    if exist_item then
        local exist_device = core.get_device(exist_item.device_id)
        if exist_device then
            dev_type_id = exist_device.device_type_id
        end
    end

    local item_factory = nil

    if dev_type_id then
        item_factory = hubplugins.weak_loadfile(string.format("HUB:zwave/scripts/model/items/%s/%s", dev_type_id, target_item_name))
    end

    if item_factory == nil then
        local item_script_name = string.format("HUB:zwave/scripts/model/items/%s/%s", "default", target_item_name)
        item_factory = assert(hubplugins.weak_loadfile(item_script_name), "Item script not found with name: " .. item_script_name)
    end
    if item_factory == nil then
        return
    end

    local item = assert(item_factory(), "Could  not  find item's filds default values. item_id: " .. item_id)

    setpoint.min_value = item.value_min.value
    setpoint.min_scale = temperature_enum.token_to_value[item.value_min.scale]
    setpoint.max_value = item.value_max.value
    setpoint.max_scale = temperature_enum.token_to_value[item.value_max.scale]

end

for _, setpoint in ipairs(cc_setpoint.setpoints) do
    if setpoint.type == setpoint_type then
        check_min_max(setpoint)
        return setpoint
    end
end
