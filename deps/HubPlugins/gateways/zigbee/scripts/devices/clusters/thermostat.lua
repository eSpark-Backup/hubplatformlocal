local cls_thermostat = hubplugins.weak_loadfile("HUB:zigbee/scripts/devices/clusters/base")()

function cls_thermostat.process_cluster(self, inclusion_context)
    local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")

    local items = {
        self:create_item(inclusion_context, "SystemMode", item_names.thermostat_mode),
        self:create_item(inclusion_context, "LocalTemperature", item_names.temp),
        self:create_item(inclusion_context, "OccupiedCoolingSetpoint", item_names.thermostat_setpoint_cooling),
        self:create_item(inclusion_context, "OccupiedHeatingSetpoint", item_names.thermostat_setpoint_heating)
    }
    if self:find_attribute("ThermostatRunningState", inclusion_context.cluster) then
        table.insert(items, self:create_item(inclusion_context, "ThermostatRunningState", item_names.thermostat_operating_state))
    end

    local settings = {}
    if self:find_attribute("LocalTemperatureCalibration", inclusion_context.cluster) then
        table.insert(settings, self:create_setting(inclusion_context, "LocalTemperatureCalibration", "thermostat_temperature_calibration"))
    end

    self:add_items_and_settings(inclusion_context, items, settings)
end

return cls_thermostat
