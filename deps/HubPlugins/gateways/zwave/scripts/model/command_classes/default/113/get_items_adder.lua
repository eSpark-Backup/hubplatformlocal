local self, notification_type = ...

local function_names = {
    appliance = "add_complicated_type_items",
    co_alarm = "add_simple_type_items",
    co2_alarm = "add_simple_type_items",
    smoke_alarm = "add_simple_type_items",
    gas_alarm = "add_simple_type_items",
    heat_alarm = "add_simple_type_items",
    siren = "add_complicated_type_items",
    light_sensor = "add_simple_type_items",
    water_valve = "add_simple_type_items",
    power_management = "add_complicated_type_items",
    clock = "add_simple_type_items",
    weather_alarm = "add_complicated_type_items",
    access_control = "add_complicated_type_items",
    home_security = "add_complicated_type_items",
    home_health = "add_simple_type_items",
    water_alarm = "add_complicated_type_items",
    system = "add_complicated_type_items",
    pest_control = "add_complicated_type_items"
}

local function_name = assert(function_names[notification_type], "[default.113.get_items_adder] items adder function is not defined for: " .. notification_type)

return function(inclusion_context, build_tree_context, device_node, items)
    self:call(function_name, inclusion_context, build_tree_context, device_node, items, notification_type)
end
