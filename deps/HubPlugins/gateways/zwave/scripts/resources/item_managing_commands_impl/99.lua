local item = ...
local item_names = require("HUB:zwave/scripts/resources/item_names")
-- User Code
local commands = {
    [item_names.user_codes] = {"USER_CODE_SET" },
    [item_names.master_code] = {"MASTER_CODE_SET"}
}

return commands[item.descriptor.item_name]
