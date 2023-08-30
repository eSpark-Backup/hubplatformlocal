local self, setpoint_item, new_value = ...

if setpoint_item.show == false then return end

if self:call("utils/is_correct_temperature", setpoint_item.value, new_value) then return end

hubplugins.weak_loadfile("HUB:zwave/scripts/set_item_value")(
    {
        item_id = setpoint_item.id,
        value = { value = new_value, scale = "fahrenheit" }
    }
)
