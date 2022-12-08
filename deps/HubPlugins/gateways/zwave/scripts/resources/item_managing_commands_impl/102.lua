local item = ...
local item_names = require( "HUB:zwave/scripts/resources/item_names")
-- Barrier Operator
local commands = {
    [item_names.barrier] = {"BARRIER_OPERATOR_SET"},
    [item_names.switch] = {"BARRIER_OPERATOR_SIGNAL_SET"}
}

return commands[item.descriptor.item_name]
