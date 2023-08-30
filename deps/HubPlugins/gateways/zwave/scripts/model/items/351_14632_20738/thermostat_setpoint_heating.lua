local item = hubplugins.weak_loadfile("HUB:zwave/scripts/model/items/default/thermostat_setpoint_heating")()

item.value_min.value = 5
item.value_max.value = 36.6

return item
