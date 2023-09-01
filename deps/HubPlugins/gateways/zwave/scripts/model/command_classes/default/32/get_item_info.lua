local self, cc_data = ...

local cc_ids = require("HUB:zwave/scripts/resources/class_ids")
local item_names = require("HUB:zwave/scripts/resources/item_names")

if cc_data.id == cc_ids.switch_binary then
    return {name = item_names.switch, to_child = false}
elseif cc_data.id == cc_ids.dimmer then
    return {name = item_names.dimmer, to_child = false}
end
-- TODO maybe add sensors
