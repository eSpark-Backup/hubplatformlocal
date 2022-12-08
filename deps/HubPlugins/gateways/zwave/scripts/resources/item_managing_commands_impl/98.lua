local item = ...
local item_names = require( "HUB:zwave/scripts/resources/item_names")
-- Door Lock
local commands = {
    [item_names.door_lock] = {"DOOR_LOCK_OPERATION_SET"},
}
return commands[item.descriptor.item_name]
