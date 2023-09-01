local self, item_descriptor, value = ...

local zwave = require("zwave")

local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local sounding_mode_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/sounding_mode_event")

if item_descriptor.class_id == class_ids.alarm_silence then
    -- Actually implementation is very basic. TODO: reimplement
    if value == sounding_mode_enum.silent then
        zwave.set_silent_sensors(item_descriptor.node_id, item_descriptor.channel_id, 0, { item_descriptor.tag } )
    elseif value == sounding_mode_enum.audible then
        zwave.set_silent_sensors(item_descriptor.node_id, item_descriptor.channel_id, 0, { } )
    end
elseif item_descriptor.class_id == class_ids.sound_switch then
    if value == sounding_mode_enum.silent then
        zwave.set_sound_switch_configuration(item_descriptor.node_id, item_descriptor.channel_id, 0, 0)
    elseif value == sounding_mode_enum.audible then
        zwave.set_sound_switch_configuration(item_descriptor.node_id, item_descriptor.channel_id, 0xFF, 0)
    end

    zwave.request_sound_switch_configuration(item_descriptor.node_id, item_descriptor.channel_id)
end
