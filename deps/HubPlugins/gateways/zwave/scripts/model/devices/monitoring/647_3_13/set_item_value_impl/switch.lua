local self, item_descriptor, value, item_id = ...

local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local item_names = require("HUB:zwave/scripts/resources/item_names")

item_descriptor.class_id = class_ids.dimmer
item_descriptor.item_name = item_names.dimmer

self:call("set_item_value_impl/" .. item_names.dimmer, item_descriptor, value and 50 or 0, item_id)
