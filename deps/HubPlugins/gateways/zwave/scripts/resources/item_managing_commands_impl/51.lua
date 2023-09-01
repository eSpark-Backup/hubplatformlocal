local item = ...
local item_names = require("HUB:zwave/scripts/resources/item_names")
-- Color Switch
local commands =  {
    [item_names.rgb_color] = {"SWITCH_COLOR_SET"},
    [item_names.rgb_color_up] = {"SWITCH_COLOR_START_LEVEL_CHANGE"},
    [item_names.rgb_color_down] = {"SWITCH_COLOR_START_LEVEL_CHANGE"},
    [item_names.rgb_color_stop] = {"SWITCH_COLOR_STOP_LEVEL_CHANGE"}
}

return commands[item.descriptor.item_name]
