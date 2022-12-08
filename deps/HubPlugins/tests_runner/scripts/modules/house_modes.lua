local house_modes = {
    modes = {
        {
            alarms_off_devices = {},
            default = true,
            disarmed_devices = {},
            id = "1",
            name = "Home"
        },
        {
            alarms_off_devices = {},
            default = true,
            disarmed_devices = {},
            id = "2",
            name = "Away"
        },
        {
            alarms_off_devices = {},
            default = true,
            disarmed_devices = {},
            id = "3",
            name = "Night"
        },
        {
            alarms_off_devices = {},
            default = true,
            disarmed_devices = {},
            id = "4",
            name = "Vacation"
        }
    },
    current = "1"
}

function house_modes.switch(mode_id)
    error( "Please, override house_modes.switch" )
end

function house_modes.cancel_switch()
    error( "Please, override house_modes.cancel_switch" )
end

function house_modes.get_current()
    return house_modes.current
end

function house_modes.get_modes()
    local mode_ids = {}

    for _, mode in ipairs(house_modes.modes) do
        table.insert(mode_ids, mode.id)
    end

    return mode_ids
end

function house_modes.get_mode(mode_id)
    for _, mode in ipairs(house_modes.modes) do
        if mode.id == mode_id then
            return mode
        end
    end
end

function house_modes.find_mode_by_name(mode_name)
    for _, mode in ipairs(house_modes.modes) do
        if mode.name == mode_name then
            return mode
        end
    end
end

return house_modes
