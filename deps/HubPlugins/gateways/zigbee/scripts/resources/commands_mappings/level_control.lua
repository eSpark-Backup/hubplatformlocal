local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")

local command_names = {
    [item_names.dimmer] = "MoveToLevelWithOnOff",
    [item_names.dimmer_up] = "MoveWithOnOff",
    [item_names.dimmer_down] = "MoveWithOnOff",
    [item_names.dimmer_stop] = "StopWithOnOff"
}

local command_params = {
    [item_names.dimmer_up] = "up",
    [item_names.dimmer_down] = "down"
}

local mapper = {}

function mapper.get_command_name(item_name, value)
    return command_names[item_name]
end

function mapper.get_command_params(item_name, value)
    if item_name == item_names.dimmer then
        -- convert value with base 100 to base 254
        return math.floor((value / (100 / 254)) + 0.5)
    else
        return command_params[item_name]
    end
end

return mapper
