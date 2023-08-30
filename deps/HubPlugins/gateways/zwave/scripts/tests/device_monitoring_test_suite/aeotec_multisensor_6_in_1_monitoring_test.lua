local tests = require("tests")
local zwave = require("zwave")
local core = require("core")
local item_names = require("HUB:zwave/scripts/resources/item_names")
local events_handler = loadfile("HUB:zwave/scripts/events/events_handling")
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/base_fixture")()

zwave.request_node_values = tests.create_mock()
core.update_item_value = tests.create_mock()

local function test_impl(params)
    local class_ids = require("HUB:zwave/scripts/resources/class_ids")

    local node = {
        id = "9000",
        manufacturer_id = 134,
        product_type = params.product_type,
        product_id = 100,
        is_battery_powered = false,

        generic_type = "GENERIC_TYPE_SENSOR_MULTILEVEL",
        specific_type = "SPECIFIC_TYPE_ROUTING_SENSOR_MULTILEVEL",
        icon_type = "ICON_TYPE_SPECIFIC_SENSOR_NOTIFICATION_MOTION_ALARM",

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
                                event_types = {
                                    "product_cover_removed"
                                },
                                type = "home_security",
                                unknown_event_types = {
                                    8
                                }
                            }
                        },
                        v1_supported = false,
                        version = 3
                    }
                }
            }
        }
    }

    fixture:add_node(node)

    local motion_item_id = storage:call("find_item_id", item_names.motion, 9000, class_ids.notification, 0, "home_security")
    local tamper_item_id = storage:call("find_item_id", item_names.tampering_cover_alarm, 9000, class_ids.notification, 0, "home_security")
    local security_item_id = storage:call("find_item_id", item_names.motion, 9000, class_ids.notification, 0, "home_security.security_tag")

    local function check_notification_impl(event, event_type, calls)
        events_handler({
            event = event,
            node_id = 9000,
            channel_id = 0,
            class_id = class_ids.notification,
            notification_type = "home_security",
            event_type = event_type,
            v1_alarm_level = 0,
            v1_alarm_type = 0
        })

        tests.assert_called_unordered(core.update_item_value, "check core.update_item_value", calls)
    end

    local function check_notification(event_type, calls)
        check_notification_impl("notification_recieved", event_type, calls)
    end

    local function check_unknown_notification(event_type, calls)
        check_notification_impl("unknown_notification_recieved", event_type, calls)
    end

    check_unknown_notification(0x08, {
        { motion_item_id, true },
        { security_item_id, true }
    })
    check_notification("motion_detected", {
        { motion_item_id, true },
        { security_item_id, true }
    })

    check_unknown_notification(0x03, {
        { tamper_item_id, "tampering_cover" }
    })
    check_notification("product_cover_removed", {
        { tamper_item_id, "tampering_cover" }
    })

    check_unknown_notification(0x00, {
        { motion_item_id, false },
        { security_item_id, false },
        { tamper_item_id, "no_tampering_cover" }
    })
    check_notification("state_idle", {
        { motion_item_id, false },
        { security_item_id, false },
        { tamper_item_id, "no_tampering_cover" }
    })

    check_unknown_notification(0xFE, {
    })

    fixture:remove_node(node)
end

tests.run_with_params(
    test_impl,
    {
        {
            value = {
                product_type = 258
            },
            description = "test with product_type = 258"
        },
        {
            value = {
                product_type = 2
            },
            description = "test with product_type = 2"
        }
    }
)
