local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")

local mappings = {
    [item_names.switch] = { [true] = "On", [false] = "Off" }
}

local mapper = {}

function mapper.get_command_name(item_name, value)
    if mappings[item_name] then
        return mappings[item_name][value]
    end
end

function mapper.get_command_params(item_name, value) end

return mapper
