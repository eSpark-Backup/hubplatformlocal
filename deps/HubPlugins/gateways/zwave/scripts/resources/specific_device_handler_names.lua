local command_class = {
    dimmable_light = { 
        [""] = "dimmable_light",
    },
    door_lock = {
        [265] = "generic_doorlock",
        [297] = "generic_doorlock",
        [144] = "generic_doorlock",
        [59] = "schlage_doorlock"
    }
}

local inclusion = {
    dimmable_light = {
        [271] = "channel_merge",
        [99] = "channel_merge",
    },
    switch = {
        [99] = "channel_merge",
    }
}

return {
    command_class = command_class,
    inclusion = inclusion,
    monitoring = command_class,
}