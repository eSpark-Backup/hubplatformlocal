local item = ...
local item_names = require( "HUB:zwave/scripts/resources/item_names")
-- Binary Switch
local commands = {
    [item_names.switch] = {"SWITCH_BINARY_SET"}
}

return commands[item.descriptor.item_name]
