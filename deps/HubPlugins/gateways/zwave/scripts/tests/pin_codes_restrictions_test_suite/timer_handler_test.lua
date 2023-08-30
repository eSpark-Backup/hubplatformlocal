local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local item_names = require("HUB:zwave/scripts/resources/item_names")
local process = loadfile("HUB:zwave/scripts/helpers/virtual_pin_codes_restrictions/process_restrictions")
local handler = loadfile("HUB:zwave/scripts/helpers/virtual_pin_codes_restrictions/timer/handler")
local core = require("core")

local fixture = loadfile("HUB:zwave/scripts/tests/fixtures/virtual_pin_codes_restrictrion_fixture")()
fixture.init()

fixture.add_device()

local pin_codes_item_id = assert(fixture.find_item_id(item_names.user_codes, class_ids.user_codes))
local daily_user_code_intervals_item_id = assert(fixture.find_item_id(item_names.daily_user_code_intervals, class_ids.user_codes))

-- init
do
    core.update_item_dictionary_value(pin_codes_item_id, "1", {code = "1111", name = "test1"})

    core.update_item_dictionary_value(
        daily_user_code_intervals_item_id,
        "1",
        {
            {
                startDateTime = "2021-02-01T01:01:01",
                stopDateTime = "2021-03-30T23:59:59"
            }
        }
    )

    fixture.set_local_time("2020-01-01T01:01:01")

    process()
end

-- test time not reached
do
    fixture.set_local_time("2021-01-01T01:01:01")

    handler()

    fixture.check_timer(true, 82740000)

    fixture.check_pin_code_status_changed({{1, "disabled", "1111"}})
end

-- test time enable reached
do
    fixture.set_local_time("2021-02-01T01:01:02")
    core.update_item_dictionary_value(pin_codes_item_id, "1", {code = "1111", name = "test1", mode = "disabled"})

    handler()

    fixture.check_timer(true, 82739000)

    fixture.check_pin_code_status_changed({{1, "enabled", "1111"}})
end

-- test time disable not reached
do
    fixture.set_local_time("2021-02-05T01:01:01")
    core.update_item_dictionary_value(pin_codes_item_id, "1", {code = "1111", name = "test1", mode = "enabled"})

    handler()

    fixture.check_pin_code_status_changed()
end

-- test time disable reached
do
    fixture.set_local_time("2021-04-30T23:59:59")

    handler()

    fixture.check_event_table(nil)
    fixture.check_timer(false)

    fixture.check_pin_code_status_changed({{1, "disabled", "1111"}})
end
