local handler, node, device_id, endpoint_id, cluster_name, attribute = ...

local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")

local base_battery_voltages = {
    AA = 1.5,
    AAA = 1.5,
    C = 1.5,
    D = 1.5,
    CR2 = 3,
    CR123A = 3
}

local function get_full_battery_voltage()
    local dev_info = handler:get_device_info(node)
    if dev_info.full_battery_voltage then
        return dev_info.full_battery_voltage
    end
    
    local battery_size_attr = handler:find_attribute(node, endpoint_id, cluster_name, "BatterySize")
    local battery_quantity_attr = handler:find_attribute(node, endpoint_id, cluster_name, "BatteryQuantity")
    if battery_size_attr and battery_quantity_attr then
        if battery_quantity_attr.formatted_value == 1 then
            if base_battery_voltages[battery_size_attr.formatted_value] then
                return base_battery_voltages[battery_size_attr.formatted_value]
            end
        end
    end
    
    print( "Using default value as max battery voltage. Please override it in devices_info.json file." )
    return 3 -- Default value
end

local function calculate_percents(voltage)
    local new_value = (voltage / get_full_battery_voltage()) * 100
    return new_value > 100 and 100 or new_value
end


-- main

if attribute.formatted_value == "undefined" then return end

if attribute.name == "BatteryPercentageRemaining" then 
    local item_id = handler:find_item_id(item_names.battery, node.id, endpoint_id, cluster_name, attribute.name)
    if item_id then
        local new_value = attribute.formatted_value / 2
        new_value = new_value > 100 and 100 or new_value
        handler:update_value_in_core(item_id, math.floor(new_value))
    end
    
elseif attribute.name == "BatteryVoltage" then 
    local item_id = handler:find_item_id(item_names.battery, node.id, endpoint_id, cluster_name, attribute.name)
    if item_id then
        local new_value = attribute.formatted_value / 2
        new_value = new_value > 100 and 100 or new_value
        handler:update_value_in_core(item_id, math.floor(calculate_percents(attribute.formatted_value / 10)))
    end
end
