local fixture = require("HUB:zwave/scripts/tests/fixtures/base_fixture")
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local zwave = require( "zwave" )
local tests = require("tests")
local item_names = require("HUB:zwave/scripts/resources/item_names")
local item_descriptor_tags = require("HUB:zwave/scripts/resources/item_descriptor_tags")

zwave.request_node_values = tests.create_mock()

local function test_impl(params)

    local channel_0 = {
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
                    }
        }
    }

    local node = {
        id = "9000",
        manufacturer_id = 297,
        product_type = params.product_type,
        product_id = 1536,
        is_battery_powered = true,

        specific_type = "ICON_TYPE_GENERIC_DOOR_LOCK_KEYPAD",
        generic_type = "GENERIC_TYPE_ENTRY_CONTROL",

        security = { "s0" },
        security_class_started = { "s0" },

        channels = { channel_0 }
    }

    -- add node
    fixture:add_node(node)

    -- check adding result
    local expected_door_lock_items = {
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
    }

    local expected_devices = {
        {
            name = params.name,
            category = "door_lock",
            items = expected_door_lock_items,
            node_id = "9000",
            battery_powered = true,
        }
    }

    local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"

    local check_zwave_node_adding_result = loadfile( test_suite_dir .. "check_zwave_node_adding_result" )

    check_zwave_node_adding_result( expected_devices )

    local request_event = {
        event = "notification_recieved",
        node_id = 9000,
        class_id = class_ids.notification,
        channel_id = 0,
        v1_alarm_type = 0x82,
        v1_alarm_level = 0X00,
        notification_type = "power_management",
        event_type = "power_has_been_applied"
    }

    -- check
    -- power_has_been_applied
    zwave.synchronize_device_time = tests.create_mock()

    events_handler(request_event)

    tests.assert_called(
        zwave.synchronize_device_time,
        "check calling synchronize device time",
        node_id )

    fixture:remove_node(node)
end

tests.run_with_params(
    test_impl,
    {
        {
            value = {
                product_type = 32770,
                name = "Touchscreen Deadbolt"
            },
            description = "test with product_type = 32770"
        },
        {
            value = {
                product_type = 32772,
                name = "Push Button Deadbolt"
            },
            description = "test with product_type = 32772"
        }
    }
)