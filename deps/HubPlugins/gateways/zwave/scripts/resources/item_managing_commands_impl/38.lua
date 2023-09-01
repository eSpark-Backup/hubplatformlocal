local item = ...
local item_names = require( "HUB:zwave/scripts/resources/item_names")
-- Multilevel Switch 
local commands = {
    [item_names.dimmer] = {"SWITCH_MULTILEVEL_SET"},
    [item_names.switch] = {"SWITCH_MULTILEVEL_SET"},
    [item_names.dimmer_up] = {"SWITCH_MULTILEVEL_START_LEVEL_CHANGE"},
    [item_names.dimmer_down] = {"SWITCH_MULTILEVEL_START_LEVEL_CHANGE"},
    [item_names.dimmer_stop] = {"SWITCH_MULTILEVEL_STOP_LEVEL_CHANGE"}
}

return commands[item.descriptor.item_name]
