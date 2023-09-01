local item = hubplugins.weak_loadfile("HUB:zwave/scripts/model/items/default/thermostat_setpoint_cooling")()

item.value_min.value = 11.1
item.value_max.value = 32.2

return item
