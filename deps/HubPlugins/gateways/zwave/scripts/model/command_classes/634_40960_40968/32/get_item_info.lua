local self, cc_data = ...

local item_names = require("HUB:zwave/scripts/resources/item_names")
return { name = item_names.switch, to_child = false }
