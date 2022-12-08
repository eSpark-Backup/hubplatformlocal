local tests = require( "tests" )
local zwave = require("zwave")
local core = require("core")
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local item_names = require("HUB:zwave/scripts/resources/item_names")
local item_tags = require("HUB:zwave/scripts/resources/item_descriptor_tags")
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local dw_enum = loadfile("HUB:zwave/scripts/resources/enums/dw_state_event")()
local tampering_cover_enum = loadfile("HUB:zwave/scripts/resources/enums/tampering_cover_events")()

local fixture = loadfile("HUB:zwave/scripts/tests/fixtures/dw_sensor_134_2_29_fixture")()

zwave.request_node_values = tests.create_mock()

local node = fixture:create_node()

fixture:add_node(node)

local binary_sensor_item_id = storage:call("find_item_id", item_names.dw_state, 9000, class_ids.sensor_binary, 0, item_tags.access_control  )
local security_threat_item_id = storage:call("find_item_id", item_names.dw_state, 9000, class_ids.sensor_binary, 0, "access_control.security_tag")
local tampering_cover_alarm_item_id = storage:call("find_item_id", item_names.tampering_cover_alarm, 9000, class_ids.notification, 0)

do -- send binary sensor
    core.update_item_value = tests.create_mock()

    events_handler( {
        event = "binary_sensor_value_updated",
        node_id = "9000",
        channel_id = 0,
        class_id = class_ids.sensor_binary,
        sensor_type = 0x00,
        value = 0xFF,
    } )

    tests.assert_called(
        core.update_item_value,
        "check if value updated on unknown sensor binary type",
        {
            { binary_sensor_item_id, dw_enum.dw_is_opened },
            { security_threat_item_id, true },
        }
    )
end

do -- send basic
    core.update_item_value = tests.create_mock()

    events_handler( {
        event = "value_updated",
        node_id = "9000",
        channel_id = 0,
        class_id = class_ids.basic,
        value = 0x00
    } )

    tests.assert_called(
        core.update_item_value,
        "check if value updated on Basic Set",
        {
            { binary_sensor_item_id, dw_enum.dw_is_closed },
            { security_threat_item_id, false },
        }
    )
end

do -- send unknown notification
    core.update_item_value = tests.create_mock()

    events_handler( {
        event = "unknown_notification_recieved",
        node_id = "9000",
        channel_id = 0,
        class_id = class_ids.notification,
        v1_alarm_type = 0x00,
        v1_alarm_level = 0xFF,
        event_type = 0,
    } )

    tests.assert_called(core.update_item_value, "", {{ tampering_cover_alarm_item_id, tampering_cover_enum.tampering_cover }})
end
