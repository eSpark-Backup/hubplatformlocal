local item = hubplugins.weak_loadfile("HUB:zwave/scripts/model/items/default/thermostat_setpoint_heating")()

item.value_min.value = 10
item.value_max.value = 31.1

return item
