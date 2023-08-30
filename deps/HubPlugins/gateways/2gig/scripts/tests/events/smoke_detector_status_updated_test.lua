local core = require("core")
local tests = require( "tests" )
local smoke_detector = require("HUB:2gig/scripts/model/devices/smoke_detector")
local smoke_alarm_states = require("HUB:2gig/scripts/enum/smoke_alarm_states")
local heat_alarm_states = require("HUB:2gig/scripts/enum/heat_alarm_states")
local freeze_alarm_states = require("HUB:2gig/scripts/enum/freeze_alarm_states")
local tampering_cover_states = require("HUB:2gig/scripts/enum/tampering_cover_states")
local send_event = loadfile("HUB:2gig/scripts/events/events_handling")
local add_device = loadfile("HUB:2gig/scripts/add_device")

core.update_item_value = tests.create_mock()

add_device({
    model = "2GIG-SMKT3-345",
    serial = 123
})

local item_ids = smoke_detector.get_item_ids(123)

local function check_event(event_params, expected_call_params)
    send_event({
        event = "smoke_detector_status_updated",
        serial = 123,
        smoke_alarm = event_params.smoke_alarm,
        heat_alarm = event_params.heat_alarm,
        freeze_alarm = event_params.freeze_alarm,
        tamper_detected = event_params.tamper_detected,
        low_battery = event_params.low_battery
    })
    tests.assert_called(core.update_item_value, "Check core.update_item_value call", expected_call_params)

    send_event({
        event = "honeywell_sensor_status_updated",
        serial = 123,
        loop1 = event_params.smoke_alarm,
        loop2 = event_params.heat_alarm,
        loop3 = event_params.freeze_alarm,
        tamper_detected = event_params.tamper_detected,
        low_battery = event_params.low_battery
    })
    tests.assert_called(core.update_item_value, "Check core.update_item_value call", expected_call_params)
end

check_event(
    {
        smoke_alarm = true,
        heat_alarm = false,
        freeze_alarm = false,
        tamper_detected = false,
        low_battery = false
    },
    {
        { item_ids.smoke_alarm_id, smoke_alarm_states.not_ok },
        { item_ids.heat_alarm_id, heat_alarm_states.ok },
        { item_ids.freeze_alarm_id, freeze_alarm_states.ok },
        { item_ids.security_threat_id, true },
        { item_ids.tampering_cover_alarm_id, tampering_cover_states.ok },
        { item_ids.battery_id, 100 },
    }
)

check_event(
    {
        smoke_alarm = false,
        heat_alarm = true,
        freeze_alarm = false,
        tamper_detected = false,
        low_battery = false
    },
    {
        { item_ids.smoke_alarm_id, smoke_alarm_states.ok },
        { item_ids.heat_alarm_id, heat_alarm_states.not_ok },
        { item_ids.freeze_alarm_id, freeze_alarm_states.ok },
        { item_ids.security_threat_id, true },
        { item_ids.tampering_cover_alarm_id, tampering_cover_states.ok },
        { item_ids.battery_id, 100 },
    }
)

check_event(
    {
        smoke_alarm = false,
        heat_alarm = false,
        freeze_alarm = true,
        tamper_detected = false,
        low_battery = false
    },
    {
        { item_ids.smoke_alarm_id, smoke_alarm_states.ok },
        { item_ids.heat_alarm_id, heat_alarm_states.ok },
        { item_ids.freeze_alarm_id, freeze_alarm_states.not_ok },
        { item_ids.security_threat_id, true },
        { item_ids.tampering_cover_alarm_id, tampering_cover_states.ok },
        { item_ids.battery_id, 100 },
    }
)

check_event(
    {
        smoke_alarm = false,
        heat_alarm = false,
        freeze_alarm = false,
        tamper_detected = true,
        low_battery = false
    },
    {
        { item_ids.smoke_alarm_id, smoke_alarm_states.ok },
        { item_ids.heat_alarm_id, heat_alarm_states.ok },
        { item_ids.freeze_alarm_id, freeze_alarm_states.ok },
        { item_ids.security_threat_id, false },
        { item_ids.tampering_cover_alarm_id, tampering_cover_states.not_ok },
        { item_ids.battery_id, 100 },
    }
)

check_event(
    {
        smoke_alarm = false,
        heat_alarm = false,
        freeze_alarm = false,
        tamper_detected = false,
        low_battery = true
    },
    {
        { item_ids.smoke_alarm_id, smoke_alarm_states.ok },
        { item_ids.heat_alarm_id, heat_alarm_states.ok },
        { item_ids.freeze_alarm_id, freeze_alarm_states.ok },
        { item_ids.security_threat_id, false },
        { item_ids.tampering_cover_alarm_id, tampering_cover_states.ok },
        { item_ids.battery_id, 15 },
    }
)
