return {
    electrical_measurement = {
        RMSVoltage = { min_interval = 0x0001, max_interval = 0x012C, reportable_change = 100 },
        RMSCurrent = { min_interval = 0x0001, max_interval = 0x012C, reportable_change = 10 },
        ActivePower = { min_interval = 0x0001, max_interval = 0x012C, reportable_change = 10 }
    },
    temperature_measurement = {
        MeasuredValue = { min_interval = 0x258, max_interval = 0xE10, reportable_change = 100 }
    },
    thermostat = {
        ThermostatRunningState = { min_interval = 0x000F, max_interval = 0x012C },
        LocalTemperatureCalibration = { min_interval = 0x0001, max_interval = 0x012C, reportable_change = 1 }
    },
    power_configuration = {
        BatteryVoltage = { min_interval = 0x0E10, max_interval = 0x1C20, reportable_change = 1 },
        BatteryPercentageRemaining = { min_interval = 0x0E10, max_interval = 0x1C20, reportable_change = 1 }
    },
    on_off = {
        OnOff = { min_interval = 0x0001, max_interval = 0x012C }
    },
    level_control = {
        CurrentLevel = { min_interval = 0x0001, max_interval = 0x012C, reportable_change = 1 }
    },
    color_control = {
        CurrentX = { min_interval = 0x0001, max_interval = 0x012C, reportable_change = 1 },
        CurrentY = { min_interval = 0x0001, max_interval = 0x012C, reportable_change = 1 },
        ColorTemperatureMireds = { min_interval = 0x0001, max_interval = 0x012C, reportable_change = 1 },
        ColorMode = { min_interval = 0x0001, max_interval = 0x012C }
    },
    ias_zone = {
        ZoneStatus = { min_interval = 0x000F, max_interval = 0x012C }
    }
}
