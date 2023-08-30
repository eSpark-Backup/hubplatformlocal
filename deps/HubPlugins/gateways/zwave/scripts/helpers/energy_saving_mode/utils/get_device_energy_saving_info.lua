local self, device_id = ...

local item_names = require("HUB:zwave/scripts/resources/item_names")

local items = self._core.get_items_by_device_id(device_id)
local info = {
    dev_id = device_id,
    energy_saving_mode_item = nil,
    setpoint_heating_item = nil,
    setpoint_cooling_item = nil,
    setpoint_item = nil
}

for _, item in ipairs(items) do
    if item.name == item_names.thermostat_energy_saving_mode then
        info.energy_saving_mode_item = item
    elseif item.name == item_names.thermostat_setpoint_heating then
        info.setpoint_heating_item = item
    elseif item.name == item_names.thermostat_setpoint_cooling then
        info.setpoint_cooling_item = item
    elseif item.name == item_names.thermostat_setpoint then
        info.setpoint_item = item
    end
end

if info.energy_saving_mode_item and
   info.setpoint_heating_item and
   info.setpoint_cooling_item and
   info.setpoint_item then
    return info
end
