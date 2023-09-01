local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/base_fixture")()
local restrictions_helper = hubplugins.object("include_restrictions_helper", "HUB:zwave/scripts/helpers")

local tests = require("tests")

function fixture.setup_restrictions(restrictions)
    local restrictions_helper = hubplugins.object("include_restrictions_helper", "HUB:zwave/scripts/helpers")
    restrictions_helper:call("restriction_added", restrictions)
end

function fixture.reset_restrictions()
    local restrictions_helper = hubplugins.object("include_restrictions_helper", "HUB:zwave/scripts/helpers")
    restrictions_helper:call("include_finished")
end

function fixture.check_restrictions(restrictions)
    tests.assert_eq(restrictions_helper:call("storage/find_include_restrictions"), restrictions)
end

function fixture.check_node_restricted(node_id, value)
    tests.assert_eq(restrictions_helper:call("storage/is_node_restricted", node_id), value)
end

function fixture.get_multisensor_node()
    local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )

    local binary_sensor_command_class = {
        id = class_ids.sensor_binary,
        sensor_types = {
            8,
            10,
            12
        }
    }

    local multilevel_sensor_command_class = {
        id = class_ids.multilevel_sensor,
        sensors = {
            {
                scales = {
                    "celsius",
                    "fahrenheit"
                },
                sensor_type = "air_temperature"
            },
            {
                scales = {
                    "percent"
                },
                sensor_type = "illuminance"
            }
        }
    }

    local notification_command_class = {
        id = class_ids.notification,
        notifications = {
            {
                event_types = {
                    "dw_is_opened",
                    "dw_is_closed"
                },
                type = "access_control",
                unknown_event_types = {}
            },
            {
                event_types = {
                    "product_cover_removed",
                    "motion_detected"
                },
                type = "home_security",
                unknown_event_types = {}
            }
        },
        v1_supported = false,
        version = 4
    }

    local channel_0 = {
        id = 0,
        classes = {
            notification_command_class,
            binary_sensor_command_class,
            multilevel_sensor_command_class
        }
    }

    return {
        id = "9000",
        manufacturer_id = 316,
        product_type = 2,
        product_id = 12,
        is_battery_powered = true,

        generic_type = "GENERIC_TYPE_SENSOR_NOTIFICATION",
        specific_type = "SPECIFIC_TYPE_NOTIFICATION_SENSOR",

        security = { "s0" },
        security_class_started = { "s0" },

        channels = { channel_0 }
    }
end

return fixture
