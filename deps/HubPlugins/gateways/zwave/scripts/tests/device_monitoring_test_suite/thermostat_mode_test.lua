local zwave = require("zwave")
local core = require("core")
local timer = require("timer")
local tests = require("tests")

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local events_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/events/events_handling")

local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local item_names = require("HUB:zwave/scripts/resources/item_names")
local mode_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/thermostat_mode")
local setpoint_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/thermostat_setpoint")
local temperature_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/temperature")

timer.set_timeout_with_id = tests.create_mock()
zwave.request_node_values = tests.create_mock()
zwave.set_setpoint_value = tests.create_mock()

local function get_item_id(class_id, item_name)
    local item_id = storage:call("find_item_id", item_name, "9000", class_id, 0)
    tests.assert_ne(item_id, nil, item_name .. " not found")

    return item_id
end

local function make_setpoint(name, min_value, max_value)
    return {
        type = setpoint_enum.token_to_value[name],
        min_value = min_value,
        min_scale = temperature_enum.token_to_value["celsius"],
        max_value = max_value,
        min_scale = temperature_enum.token_to_value["celsius"],
    }
end

local channel_0 = {
    id = 0,
    classes = {
        {
            id = class_ids.thermostat_mode,
            modes = {
                mode_enum.token_to_value["off"],
                mode_enum.token_to_value["heat"],
                mode_enum.token_to_value["cool"],
                mode_enum.token_to_value["auto"],
                mode_enum.token_to_value["aux_mode"],
                mode_enum.token_to_value["resume"],
                mode_enum.token_to_value["fan_only"],
                mode_enum.token_to_value["furnace"],
                mode_enum.token_to_value["dry_air"],
                mode_enum.token_to_value["moist_air"],
                mode_enum.token_to_value["auto_change_over"],
                mode_enum.token_to_value["energy_saving_heat"],
                mode_enum.token_to_value["energy_saving_cool"],
                mode_enum.token_to_value["away"],
                mode_enum.token_to_value["reserved"],
                mode_enum.token_to_value["full_power"],
                mode_enum.token_to_value["manufacturer_specifc"],
            }
        },
        {
            id = class_ids.thermostat_setpoint,
            version = 3,
            setpoints = {
                make_setpoint("heating", 10, 50),
                make_setpoint("cooling", 20, 60),
                make_setpoint("furnace", 30, 70),
                make_setpoint("dry_air", 40, 80),
                make_setpoint("moist_air", 50, 90),
                make_setpoint("auto_changeover", 60, 100),
                make_setpoint("energy_save_heating", 70, 110),
                make_setpoint("energy_save_cooling", 80, 120),
                make_setpoint("away_heating", 90, 130),
                make_setpoint("away_cooling", 100, 140),
                make_setpoint("full_power", 110, 150),
            }
        }
    }
}

local node = {
    id = "9000",
    manufacturer_id = 0,
    product_type = 1,
    product_id = 2,
    is_battery_powered = true,

    specific_type = "",
    generic_type = "",

    security = { "none" },
    security_class_started = { "none" },

    channels = { channel_0 }
}

-- prepare zwave
zwave.add_node(node)

-- add node
events_handler({
    event = "node_added",
    node_id = 9000
})

local thermostat_mode_item_id = get_item_id(class_ids.thermostat_mode, item_names.thermostat_mode)
local thermostat_setpoint_item_id = get_item_id(class_ids.thermostat_setpoint, item_names.thermostat_setpoint)
local thermostat_setpoint_heating_item_id = get_item_id(class_ids.thermostat_setpoint, item_names.thermostat_setpoint_heating)
local thermostat_setpoint_cooling_item_id = get_item_id(class_ids.thermostat_setpoint, item_names.thermostat_setpoint_cooling)

local function activate_mode(mode)
    events_handler({
        event = "thermostat_mode_updated",
        node_id = 9000,
        channel_id = 0,
        class_id = class_ids.thermostat_mode,
        mode = mode_enum.token_to_value[mode]
    })
end

local function check_items_shown(mode, item_ids)
    activate_mode(mode)

    local item_visibilities = {
        [thermostat_setpoint_item_id] = false,
        [thermostat_setpoint_heating_item_id] = false,
        [thermostat_setpoint_cooling_item_id] = false
    }

    for _, item_id in pairs(item_ids) do
        item_visibilities[item_id] = true
    end

    for item_id, is_shown in pairs(item_visibilities) do
        local item = core.get_item(item_id)
        tests.assert_eq(item.show, is_shown, item.name .. " in " .. mode .. " mode should be " .. (is_shown and "visible" or "hidden"))
    end
end

check_items_shown("off", {})
check_items_shown("heat", { thermostat_setpoint_heating_item_id })
check_items_shown("cool", { thermostat_setpoint_cooling_item_id })
check_items_shown("auto", { thermostat_setpoint_heating_item_id, thermostat_setpoint_cooling_item_id })
check_items_shown("aux_mode", { thermostat_setpoint_heating_item_id })
check_items_shown("resume", {})
check_items_shown("fan_only", {})
check_items_shown("furnace", { thermostat_setpoint_item_id })
check_items_shown("dry_air", { thermostat_setpoint_item_id })
check_items_shown("moist_air", { thermostat_setpoint_item_id })
check_items_shown("auto_change_over", { thermostat_setpoint_item_id })
check_items_shown("energy_saving_heat", { thermostat_setpoint_heating_item_id })
check_items_shown("energy_saving_cool", { thermostat_setpoint_cooling_item_id })
check_items_shown("away", { thermostat_setpoint_heating_item_id, thermostat_setpoint_cooling_item_id })
check_items_shown("reserved", {})
check_items_shown("full_power", { thermostat_setpoint_item_id })
check_items_shown("manufacturer_specific", { thermostat_setpoint_heating_item_id, thermostat_setpoint_cooling_item_id })
