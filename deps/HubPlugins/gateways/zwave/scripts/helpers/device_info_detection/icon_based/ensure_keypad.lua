local icon_type = ...

if icon_type == "ICON_TYPE_SPECIFIC_ENTRY_CONTROL_KEYPAD_0_9"
or icon_type == "ICON_TYPE_SPECIFIC_ENTRY_CONTROL_KEYPAD_0_9_OK_CANCEL"
or icon_type == "ICON_TYPE_SPECIFIC_ENTRY_CONTROL_KEYPAD_0_9_OK_CANCEL_HOME_STAY_AWAY" then
    return {
        name = "Keypad",
        category = "keypad"
    } 
end
