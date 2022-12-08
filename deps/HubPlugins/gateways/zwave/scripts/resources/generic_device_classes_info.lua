local class_ids = require("HUB:zwave/scripts/resources/class_ids")

local switch_multilevel_generic_type_info = {
    category = "dimmable_light",
    subcategory = "dimmable_bulb",
    mandatory_command_classes = { class_ids.dimmer },
    specific_classes = {
        ["SPECIFIC_TYPE_NOT_USED"] = true,
        ["SPECIFIC_TYPE_POWER_SWITCH_MULTILEVEL"] = true,
    }
}

local doorlock_generic_type_info = {
    category = "door_lock",
    mandatory_command_classes = { class_ids.door_lock },
    specific_classes = {
        ["SPECIFIC_TYPE_DOOR_LOCK"] = true,
        ["SPECIFIC_TYPE_ADVANCED_DOOR_LOCK"] = true,
        ["SPECIFIC_TYPE_SECURE_KEYPAD_DOOR_LOCK"] = true,
        ["SPECIFIC_TYPE_SECURE_KEYPAD_DOOR_LOCK_DEADBOLT"] = true
    }
}

return {
    ["GENERIC_TYPE_SWITCH_MULTILEVEL" ] = switch_multilevel_generic_type_info,
    ["GENERIC_TYPE_ENTRY_CONTROL"] = doorlock_generic_type_info
}