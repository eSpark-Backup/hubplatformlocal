local icon_type = ...

if icon_type == "ICON_TYPE_SPECIFIC_ON_OFF_WALL_SWITCH_DOOR_BELL"
or icon_type == "ICON_TYPE_SPECIFIC_SOUND_SWITCH_DOORBELL" then
    return {
        name = "Door Bell",
        category = "doorbell"
    } 
end
