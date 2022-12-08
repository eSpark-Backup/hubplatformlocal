local zwave = require("zwave")
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local item_value_type = require("HUB:zwave/scripts/resources/item_value_type")
local tests = require("tests")
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local core = require("core")

zwave.request_node_values = tests.create_mock()

zwave.set_year_day_entry_lock_schedule = tests.create_mock()
zwave.request_year_day_entry_lock_schedule = tests.create_mock()

local cc_user_code = {
    id = class_ids.user_codes,
    supported_users_count = 250
}

local cc_schedule_entry_lock= {
    id = class_ids.schedule_entry_lock,
    year_day_slots = 10,
    week_day_slots = 0,
    daily_repeating_slots = 0
}

local channel_0 = {
    id = 0,
    classes = { cc_user_code, cc_schedule_entry_lock }
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

zwave.add_node( node )

-- add node
events_handler( {
    event = "node_added",
    node_id = 9000
} )

local item_id = storage:call("find_item_id", 
    "daily_user_code_intervals", 
    "9000", 
    class_ids.schedule_entry_lock, 
    channel_0.id )

tests.assert_ne(item_id, nil, "check item id")

local external_setter = loadfile( "HUB:zwave/scripts/set_item_dictionary_value" )
local external_remover = loadfile( "HUB:zwave/scripts/remove_item_dictionary_value" )

local zwave_value_1 = { 
    startYear = 2020,
    startMonth = 3,
    startDay = 10,
    startHour = 11,
    startMinute = 50,
    stopYear = 2021,
    stopMonth = 4,
    stopDay = 12,
    stopHour = 14,
    stopMinute = 40
}

local zwave_value_2 = { 
    startYear = 2024,
    startMonth = 3,
    startDay = 10,
    startHour = 11,
    startMinute = 51,
    stopYear = 2025,
    stopMonth = 4,
    stopDay = 12,
    stopHour = 0,
    stopMinute = 41
}

local zwave_value_5 = { 
    startYear = 2027,
    startMonth = 1,
    startDay = 10,
    startHour = 11,
    startMinute = 50,
    stopYear = 2028,
    stopMonth = 1,
    stopDay = 12,
    stopHour = 0,
    stopMinute = 0
}

local function send_slot_change_event(slot, value)
    events_handler( {
        event = "year_day_entry_lock_schedule_updated",
        node_id = "9000",
        channel_id = 0,
        class_id = class_ids.schedule_entry_lock,
        user_id = 2,
        slot_id = slot,
        value = value
    } )
end

local function check_set_and_request_called(slot, zwave_value, description)
    tests.assert_called(
        zwave.set_year_day_entry_lock_schedule, 
        "check set year day entry lock schedule after " .. description, 
        { { "9000", 0, 2, slot, zwave_value } })

    tests.assert_called(
        zwave.request_year_day_entry_lock_schedule, 
        "check year day repeating entry lock schedule after " .. description, 
        { { "9000", 0, 2, slot } })
end

local function check_set_and_request_not_called(description)
    tests.assert_called(
        zwave.set_year_day_entry_lock_schedule, 
        "check don't call set year day entry lock schedule after " .. description, 
        { })

    tests.assert_called(
        zwave.request_year_day_entry_lock_schedule, 
        "check don't request year day entry lock schedule after " .. description, 
        { })
end

do -- set slot values from ui
    external_setter({
            item_id = item_id,
            key = "2",
            value_type = "array.daily_interval",
            value = {
                {
                    startDateTime = "2020-03-10T11:50:00",
                    stopDateTime = "2021-04-12T14:40:00",
                },
                {
                    startDateTime = "2024-03-10T11:51:40",
                    stopDateTime = "2025-04-12T00:41:10",
                },
            }
        })

    check_set_and_request_called(1, zwave_value_1, "external setting")
    tests.assert_eq(core.get_item(item_id).value, {}, "check empty dictionary after external value set")
end

do -- slot 1 changed
    send_slot_change_event(1, zwave_value_1)

    tests.assert_eq(
        core.get_item(item_id).value, 
        {
            ["2"] = {
                {
                    startDateTime = "2020-03-10T11:50:00",
                    stopDateTime = "2021-04-12T14:40:00"
                }
            }
        }, 
        "check value after first report from zwave")

    check_set_and_request_called(2, zwave_value_2, "first slot changing")
end

do -- unexpected slot changes
    send_slot_change_event(5, zwave_value_5)

    zwave_value_1.stopYear = 2030
    send_slot_change_event(1, zwave_value_1)

    tests.assert_eq(
        core.get_item(item_id).value, {
            ["2"] = {
                {
                    startDateTime = "2020-03-10T11:50:00",
                    stopDateTime = "2030-04-12T14:40:00"
                },
                {
                    startDateTime = "2027-01-10T11:50:00",
                    stopDateTime = "2028-01-12T00:00:00"
                }
            }
        }, 
        "check value after unexpected slots changes")

    check_set_and_request_not_called("unexpected slots change")
end


do -- 2nd slot changes
    send_slot_change_event(2, zwave_value_2)

    tests.assert_eq(
        core.get_item(item_id).value, {
            ["2"] = {
                {
                    startDateTime = "2020-03-10T11:50:00",
                    stopDateTime = "2030-04-12T14:40:00"
                },
                {
                    startDateTime = "2024-03-10T11:51:00",
                    stopDateTime = "2025-04-12T00:41:00"
                },
                {
                    startDateTime = "2027-01-10T11:50:00",
                    stopDateTime = "2028-01-12T00:00:00"
                }
            }
        }, 
        "check value after expected 2nd slot change")

    check_set_and_request_not_called("expected 2nd (last) slot change")
end


do -- remove slot values from ui
    external_remover({
            item_id = item_id,
            key = "2"
        })
    check_set_and_request_called(1, nil, "external removing")

    send_slot_change_event(1, nil)
    check_set_and_request_called(2, nil, "first slot removing report")

    send_slot_change_event(2, nil)
    check_set_and_request_called(5, nil, "second slot removing report")

    tests.assert_ne(core.get_item(item_id).value, {}, "check dictionary is not empty")

    send_slot_change_event(5, nil)
    check_set_and_request_not_called("all slots removing")

    tests.assert_eq(core.get_item(item_id).value, {}, "check dictionary is empty after all removing reports")
end
