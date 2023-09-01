local self, inclusion_context, build_tree_context, device_node, item_name, item_tag = ...

local item_names = require("HUB:zwave/scripts/resources/item_names")

local function create_item(...--[[tokens]])
    local item = self:call("create_item", inclusion_context, build_tree_context, device_node, item_name, item_tag)
    hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/append_to_table")(item.enum, ...)

    return item
end

-- main

if item_name == item_names.smoke_alarm then
    local smoke_alarm_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/smoke_alarm_event")
    return create_item(
        smoke_alarm_enum.no_smoke,
        smoke_alarm_enum.smoke_detected
    )

elseif item_name == item_names.co_alarm then
    local co_alarm_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/co_alarm_event")
    return create_item(
        co_alarm_enum.no_co,
        co_alarm_enum.co_detected
    )

elseif item_name == item_names.co2_alarm then
    local co2_alarm_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/co2_alarm_event")
    return create_item(
        co2_alarm_enum.no_co2,
        co2_alarm_enum.co2_detected
    )

elseif item_name == item_names.water_leak_alarm then
    local water_leak_alarm_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/water_leak_alarm_event")
    return create_item(
        water_leak_alarm_enum.no_water_leak,
        water_leak_alarm_enum.water_leak_detected
    )

elseif item_name == item_names.heat_alarm then
    local heat_alarm_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/heat_alarm_event")
    return create_item(
        heat_alarm_enum.heat_ok,
        heat_alarm_enum.overheat_detected
    )

else
    print("Cannot process following alarm sensor: " .. item_name)
    return nil
end

