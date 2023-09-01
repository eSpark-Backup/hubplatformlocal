local core = require("core")
local tests = require( "tests" )
local door_window_sensor = require("HUB:2gig/scripts/model/devices/door_window_sensor")
local door_window_states = require("HUB:2gig/scripts/enum/door_window_states")
local tampering_cover_states = require("HUB:2gig/scripts/enum/tampering_cover_states")
local storage_utils = require("HUB:2gig/scripts/helpers/storage_utils")
local send_event = loadfile("HUB:2gig/scripts/events/events_handling")
local add_device = loadfile("HUB:2gig/scripts/add_device")

core.update_item_value = tests.create_mock()

add_device({
    model = "2GIG-DW10-345",
    serial = 123
})

local item_ids = door_window_sensor.get_item_ids(123)

local function check_event(event_params, expected_call_params)
    send_event({
        event = "door_window_sensor_status_updated",
        serial = 123,
        reed_closed = event_params.reed_closed,
        tamper_detected = event_params.tamper_detected,
        external_contact = event_params.external_contact,
        low_battery = event_params.low_battery
    })
    tests.assert_called(core.update_item_value, "Check core.update_item_value call", expected_call_params)

    send_event({
        event = "honeywell_sensor_status_updated",
        serial = 123,
        loop1 = event_params.external_contact,
        loop2 = event_params.reed_closed,
        tamper_detected = event_params.tamper_detected,
        low_battery = event_params.low_battery
    })
    tests.assert_called(core.update_item_value, "Check core.update_item_value call", expected_call_params)
end

-- test tamper
check_event(
    {
        reed_closed = false,
        tamper_detected = true,
        external_contact = false,
        low_battery = false
    },
    {
        { item_ids.door_window_state_id, door_window_states.dw_is_closed },
        { item_ids.security_threat_id, false },
        { item_ids.tampering_cover_alarm_id, tampering_cover_states.not_ok },
        { item_ids.battery_id, 100 },
    }
)

-- test battery
check_event(
    {
        reed_closed = false,
        tamper_detected = false,
        external_contact = false,
        low_battery = true
    },
    {
        { item_ids.door_window_state_id, door_window_states.dw_is_closed },
        { item_ids.security_threat_id, false },
        { item_ids.tampering_cover_alarm_id, tampering_cover_states.ok },
        { item_ids.battery_id, 15 },
    }
)

-- prepare for dw state tests
local function test_dw_state(reed_closed, external_contact, dw_state)
    check_event(
        {
            reed_closed = reed_closed,
            tamper_detected = false,
            external_contact = external_contact,
            low_battery = false
        },
        {
            { item_ids.door_window_state_id, dw_state },
            { item_ids.security_threat_id, dw_state ~= door_window_states.dw_is_closed },
            { item_ids.tampering_cover_alarm_id, tampering_cover_states.ok },
            { item_ids.battery_id, 100 },
        }
    )
end

local setting_id = storage_utils.find_setting_id( storage_utils.find_device_id(123), "dw_detection_source" )
tests.assert_ne(setting_id, nil)

-- test magnet contact
core.set_setting_value(setting_id, "magnet_contact", "synced")

test_dw_state(false, true, door_window_states.dw_is_closed)
test_dw_state(false, false, door_window_states.dw_is_closed)
test_dw_state(true, true, door_window_states.dw_is_opened)
test_dw_state(true, false, door_window_states.dw_is_opened)

-- test external contact
core.set_setting_value(setting_id, "external_contact", "synced")

test_dw_state(false, false, door_window_states.dw_is_closed)
test_dw_state(true, false, door_window_states.dw_is_closed)
test_dw_state(false, true, door_window_states.dw_is_opened)
test_dw_state(true, true, door_window_states.dw_is_opened)

-- test mixed optimistic
core.set_setting_value(setting_id, "mixed_optimistic", "synced")

test_dw_state(false, false, door_window_states.dw_is_closed)
test_dw_state(true, false, door_window_states.dw_is_closed)
test_dw_state(false, true, door_window_states.dw_is_closed)
test_dw_state(true, true, door_window_states.dw_is_opened)

-- test mixed pessimistic
core.set_setting_value(setting_id, "mixed_pessimistic", "synced")

test_dw_state(false, false, door_window_states.dw_is_closed)
test_dw_state(true, false, door_window_states.dw_is_opened)
test_dw_state(false, true, door_window_states.dw_is_opened)
test_dw_state(true, true, door_window_states.dw_is_opened)
