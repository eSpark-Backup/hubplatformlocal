local tests = require( "tests" )
local zwave = require("zwave")
local item_names = require("HUB:zwave/scripts/resources/item_names")
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local item_descriptor_tags = require("HUB:zwave/scripts/resources/item_descriptor_tags")
local events_handler = hubplugins.weak_loadfile( "HUB:zwave/scripts/events/events_handling" )

zwave.request_node_values = tests.create_mock()

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

    channels = {
        {
            id = 0,
            classes = {
                {
                    id = class_ids.notification,
                    notifications = {
                        {
                            type = "power_management",
                            event_types = {
                                "power_has_been_applied",
                                "ac_mains_disconnected",
                                "ac_mains_reconnected",
                                "surge_detected",
                                "voltage_drop_drift",
                                "over_current_detected",
                                "over_voltage_detected",
                                "over_load_detected",
                                "load_error",
                                "replace_battery_soon",
                                "replace_battery_now",
                                "battery_fluid_is_low",
                                "battery_is_charging",
                                "battery_is_fully_charged",
                                "charge_battery_soon",
                                "charge_battery_now",
                                "backup_battery_is_low",
                                "backup_battery_is_disconnected",
                            }
                        }
                    }
                },
                {
                    id = class_ids.switch_binary -- NOTE: could be either switch_multilevel, switch_color or another simple command class
                }
            }
        }
    }
}

-- prepare zwave
zwave.add_node(node)

-- add node
events_handler({
    event = "node_added",
    node_id = 9000
})

local expected_devices = {
    {
        node_id = "9000",
        battery_powered = true,
        security = "no",
        name = "Siren",
        category = "siren",
        items = {
            {
                name = item_names.switch,
                cc_id = class_ids.switch_binary,
                channel_id = 0,
            },
            {
                name = item_names.power_state,
                cc_id = class_ids.notification,
                tag = item_descriptor_tags.power_management,
                channel_id = 0,
            },
            {
                name = item_names.ac_state,
                cc_id = class_ids.notification,
                tag = item_descriptor_tags.power_management,
                channel_id = 0,
            },
            {
                name = item_names.power_surge_state,
                cc_id = class_ids.notification,
                tag = item_descriptor_tags.power_management,
                channel_id = 0,
            },
            {
                name = item_names.voltage_drop_drift_state,
                cc_id = class_ids.notification,
                tag = item_descriptor_tags.power_management,
                channel_id = 0,
            },
            {
                name = item_names.over_current_state,
                cc_id = class_ids.notification,
                tag = item_descriptor_tags.power_management,
                channel_id = 0,
            },
            {
                name = item_names.over_voltage_state,
                cc_id = class_ids.notification,
                tag = item_descriptor_tags.power_management,
                channel_id = 0,
            },
            {
                name = item_names.over_load_state,
                cc_id = class_ids.notification,
                tag = item_descriptor_tags.power_management,
                channel_id = 0,
            },
            {
                name = item_names.load_error_state,
                cc_id = class_ids.notification,
                tag = item_descriptor_tags.power_management,
                channel_id = 0,
            },
            {
                name = item_names.battery_maintenance_state,
                cc_id = class_ids.notification,
                tag = item_descriptor_tags.power_management,
                channel_id = 0,
            },
            {
                name = item_names.battery_charging_state,
                cc_id = class_ids.notification,
                tag = item_descriptor_tags.power_management,
                channel_id = 0,
            },
            {
                name = item_names.battery,
                cc_id = class_ids.notification,
                tag = item_descriptor_tags.power_management,
                channel_id = 0,
            },
            {
                name = item_names.battery_backup,
                cc_id = class_ids.notification,
                tag = item_descriptor_tags.power_management,
                channel_id = 0,
            },
        },

        subdevices = {}
    }
}

local check_context = {
    descriptors = {},
    item_ids = {},
    device_ids = {},
}

local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"
loadfile(test_suite_dir .. "check_zwave_node_adding_result")(expected_devices)

-- remove node
events_handler( {
    event = "node_removed",
    node_id = "9000",
    manufacturer_id = 0,
    product_type = 1,
    product_id = 2,
} )

-- check removing result
loadfile(test_suite_dir .. "check_zwave_node_removing_result")(check_context)
