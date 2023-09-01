local zone_status_enum = hubplugins.weak_loadfile("HUB:zigbee/scripts/resources/enums/zone_status")()

local zone_status_to_panel_status = {
    [zone_status_enum.disarmed] = "panel_disarmed",
    [zone_status_enum.stay] = "armed_stay",
    [zone_status_enum.night] = "armed_night",
    [zone_status_enum.away] = "armed_away"
}

local arm_mode_to_zone_status = {
    disarm = zone_status_enum.disarmed,
    arm_day_home_zones_only = zone_status_enum.stay,
    arm_night_home_zones_only = zone_status_enum.night,
    arm_all_zones = zone_status_enum.away
}

local zone_status_to_arm_response = {
    [zone_status_enum.disarmed] = "all_zones_disarmed",
    [zone_status_enum.stay] = "only_day_home_zones_armed",
    [zone_status_enum.night] = "only_night_sleep_zones_armed",
    [zone_status_enum.away] = "all_zones_armed"
}

local arm_mode_to_house_mode = {
    disarm = "1",
    arm_day_home_zones_only = "1",
    arm_night_home_zones_only = "3",
    arm_all_zones = "2"
}

local house_mode_to_zone_status = {
    ["1"] = zone_status_enum.disarmed,
    ["3"] = zone_status_enum.night,
    ["2"] = zone_status_enum.away
}

return {
    zone_status_to_panel_status = zone_status_to_panel_status,
    arm_mode_to_zone_status = arm_mode_to_zone_status,
    zone_status_to_arm_response = zone_status_to_arm_response,
    arm_mode_to_house_mode = arm_mode_to_house_mode,
    house_mode_to_zone_status = house_mode_to_zone_status
}
