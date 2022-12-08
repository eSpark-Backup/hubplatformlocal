local self, item_descriptor, value = ...

local zwave = require("zwave")

local item_names = require("HUB:zwave/scripts/resources/item_names")

if value then
    if item_descriptor.tag == item_names.dimmer then
        zwave.set_value(item_descriptor.node_id, item_descriptor.class_id, item_descriptor.channel_id, 0xFF)
    else
        error("How to handle goto_favorite for item " .. tostring(item_descriptor.tag) .. "?")
    end
end

hubplugins.weak_loadfile("HUB:zwave/scripts/request_value")(item_descriptor)
