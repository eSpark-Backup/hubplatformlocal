local item = ...
local item_names = require( "HUB:zwave/scripts/resources/item_names")
-- Basic
local commands =  {
    [item_names.switch] = {"BASIC_SET"},
    [item_names.basic] = {"BASIC_SET"},
    [item_names.dimmer] = {"BASIC_SET"}
}

return commands[item.descriptor.item_name]
