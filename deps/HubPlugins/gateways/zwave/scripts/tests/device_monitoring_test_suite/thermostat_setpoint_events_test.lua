local zwave = require("zwave")
local core = require("core")
local timer = require("timer")
local tests = require("tests")

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local events_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/events/events_handling")
local external_setter = hubplugins.weak_loadfile("HUB:zwave/scripts/set_item_value")

local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local item_names = require("HUB:zwave/scripts/resources/item_names")
local mode_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/thermostat_mode")
local setpoint_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/thermostat_setpoint")
local temperature_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/temperature")

timer.set_timeout = tests.create_mock()
zwave.request_node_values = tests.create_mock()
zwave.set_setpoint_value = tests.create_mock()

local function get_item_id(class_id, item_name)
    local item_id = storage:call("find_item_id", item_name, "9000", class_id, 0)
    tests.assert_ne(item_id, nil, item_name .. " not found")

    return item_id
end

local function set_item_value(type, value, scale)
    events_handler({
        event = "thermostat_setpoint_value_updated",
        node_id = 9000,
        channel_id = 0,
        class_id = class_ids.thermostat_setpoint,
        type = setpoint_enum.token_to_value[type],
        value = value,
        scale = scale
    })
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
                mode_enum.token_to_value["aux"],
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
    core.update_item_value(thermostat_mode_item_id, mode)
end

local function check_items_of_setpoints_in_mode(mode, setpoint_items)
    activate_mode(mode)

    for setpoint_name, item_id in ipairs(setpoint_items) do
        local value = 123
        local scale = temperature_enum.token_to_value["celsius"]

        set_item_value(setpoint_name, value, scale)

        local item = core.get_item(item_id)
        tests.assert_eq(item.value, { value = value, scale })
    end
end

do
    check_items_of_setpoints_in_mode("off", {
    })

    check_items_of_setpoints_in_mode("heat", {
        heating = thermostat_setpoint_heating_item_id
    })

    check_items_of_setpoints_in_mode("cool", {
        cooling = thermostat_setpoint_cooling_item_id
    })

    check_items_of_setpoints_in_mode("auto", {
        heating = thermostat_setpoint_heating_item_id,
        cooling = thermostat_setpoint_cooling_item_id
    })

    check_items_of_setpoints_in_mode("aux", {
        heating = thermostat_setpoint_heating_item_id
    })

    check_items_of_setpoints_in_mode("resume", {
    })

    check_items_of_setpoints_in_mode("fan_only", {
    })

    check_items_of_setpoints_in_mode("reserved", {
    })

    check_items_of_setpoints_in_mode("furnace", {
        furnace = thermostat_setpoint_item_id
    })

    check_items_of_setpoints_in_mode("dry_air", {
        dry_air = thermostat_setpoint_item_id
    })

    check_items_of_setpoints_in_mode("moist_air", {
        moist_air = thermostat_setpoint_item_id
    })

    check_items_of_setpoints_in_mode("auto_change_over", {
        auto_changeover = thermostat_setpoint_item_id
    })

    check_items_of_setpoints_in_mode("energy_saving_heat", {
        energy_save_heating = thermostat_setpoint_heating_item_id
    })

    check_items_of_setpoints_in_mode("energy_saving_cool", {
        energy_save_cooling = thermostat_setpoint_cooling_item_id
    })

    check_items_of_setpoints_in_mode("away", {
        away_heating = thermostat_setpoint_heating_item_id,
        away_cooling = thermostat_setpoint_cooling_item_id
    })

    check_items_of_setpoints_in_mode("full_power", {
        full_power = thermostat_setpoint_item_id
    })
end
