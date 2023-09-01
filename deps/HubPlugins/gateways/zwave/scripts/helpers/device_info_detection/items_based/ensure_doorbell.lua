local items = ...

local item_names = require("HUB:zwave/scripts/resources/item_names")

if items[item_names.battery] ~= nil
and items[item_names.sound_playback] ~= nil and items[item_names.siren_alarm] == nil then
    return {
        name = "Dorrbell",
        category = "doorbell",
    } 
end
