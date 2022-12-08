local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local item_names = require("HUB:zwave/scripts/resources/item_names")
local process = loadfile("HUB:zwave/scripts/helpers/virtual_pin_codes_restrictions/process_restrictions")
local core = require("core")
local tests = require("tests")
local zwave = require("zwave")
local timer = require("timer")

local fixture = loadfile("HUB:zwave/scripts/tests/fixtures/virtual_pin_codes_restrictrion_fixture")()
fixture.init()

local function check_restriction_seted(params)
    local params_to_check = {}

    for _, param in ipairs(params) do
        local value = (not param[2]) and  {
        weekDays = { "monday" },
        startHour = 1,
        startMinute = 1,
        durationHour = 0,
        durationMinute = 0
    } or nil
        table.insert(params_to_check, { "9000", 0, param[1], 1, value })
    end

    zwave.set_daily_repeating_entry_lock_schedule = tests.create_mock()

    timer.add_time((#params_to_check * 2000) + 2)

    tests.assert_called_unordered(zwave.set_daily_repeating_entry_lock_schedule, "Check pin code status changed", params_to_check)
end

local function test_impl(params)
    fixture.add_device(9000, params.manufacturer_id, params.product_type, params.product_id)

    local pin_codes_item_id = assert(fixture.find_item_id(item_names.user_codes, class_ids.user_codes))
    local daily_user_code_intervals_item_id = assert(fixture.find_item_id(item_names.daily_user_code_intervals, class_ids.user_codes))

    -- add pin code
    core.update_item_dictionary_value(pin_codes_item_id, "1", {code = "1111", name = "test1"})

    -- test no restrictions
    do
        process()

        fixture.check_item_value(
            pin_codes_item_id,
            {
                ["1"] = {code = "1111", name = "test1"},
            }
        )

        fixture.check_event_table(nil)
        fixture.check_timer(false)
    end

    -- test restriction in past
    do
        core.update_item_dictionary_value(
            daily_user_code_intervals_item_id,
            "1",
            {
                {
                    startDateTime = "2021-01-01T01:01:01",
                    stopDateTime = "2021-02-30T23:59:59"
                }
            }
        )

        fixture.set_local_time("2022-01-01T01:01:01")

        process()

        fixture.check_event_table(nil)
        fixture.check_timer(false)

        check_restriction_seted({{1, false}})
    end

    -- test restriction in future
    do
        core.update_item_dictionary_value(pin_codes_item_id, "1", {code = "1111", name = "test1", mode = "enabled"})

        core.update_item_dictionary_value(
            daily_user_code_intervals_item_id,
            "1",
            {
                {
                    startDateTime = "2021-02-01T06:05:07",
                    stopDateTime = "2021-03-30T23:59:59"
                }
            }
        )

        fixture.set_local_time("2021-01-01T01:01:01")

        process()

        fixture.check_event_table({
            year = 2021, mounth = 2, day = 1,
            hour = 6, minute = 5, second = 7
        })

        fixture.check_timer(true, 82740000)

        check_restriction_seted({{1, false}})
    end

    -- test restriction enabled
    do
        core.update_item_dictionary_value(pin_codes_item_id, "1", {code = "1111", name = "test1", mode = "disabled"})

        core.update_item_dictionary_value(
            daily_user_code_intervals_item_id,
            "1",
            {
                {
                    startDateTime = "2021-02-01T06:05:07",
                    stopDateTime = "2021-03-30T23:59:59"
                }
            }
        )

        fixture.set_local_time("2021-02-05T01:01:01")

        process()

        fixture.check_event_table({
            year = 2021, mounth = 3, day = 30,
            hour = 23, minute = 59, second = 59
        })

        fixture.check_timer(true, 82740000)

        check_restriction_seted({{1, true}})
    end

    -- test two restrictions disabled
    do
        core.update_item_dictionary_value(pin_codes_item_id, "1", {code = "1111", name = "test1", mode = "enabled"})

        core.update_item_dictionary_value(
            daily_user_code_intervals_item_id,
            "1",
            {
                {
                    startDateTime = "2021-02-01T06:05:07",
                    stopDateTime = "2021-03-30T23:59:59"
                },
                {
                    startDateTime = "2023-02-01T06:05:07",
                    stopDateTime = "2023-03-30T23:59:59"
                }
            }
        )

        fixture.set_local_time("2022-02-05T01:01:01")

        process()

        fixture.check_event_table({
            year = 2023, mounth = 2, day = 1,
            hour = 6, minute = 5, second = 7
        })

        fixture.check_timer(true, 82740000)

        check_restriction_seted({{1, false}})
    end

    -- test two restrictions one enabled
    do
        core.update_item_dictionary_value(pin_codes_item_id, "1", {code = "1111", name = "test1", mode = "disabled"})

        core.update_item_dictionary_value(
            daily_user_code_intervals_item_id,
            "1",
            {
                {
                    startDateTime = "2021-02-01T06:05:07",
                    stopDateTime = "2021-03-30T23:59:59"
                },
                {
                    startDateTime = "2023-02-01T06:05:07",
                    stopDateTime = "2023-03-30T23:59:59"
                }
            }
        )

        fixture.set_local_time("2021-02-05T01:01:01")

        process()

        fixture.check_event_table({
            year = 2021, mounth = 3, day = 30,
            hour = 23, minute = 59, second = 59
        })

        fixture.check_timer(true, 82740000)

        check_restriction_seted({{1, true}})
    end

    -- test two pin codes one enabled
    do
        core.update_item_dictionary_value(
            daily_user_code_intervals_item_id,
            "1",
            {
                {
                    startDateTime = "2021-02-01T06:05:07",
                    stopDateTime = "2021-03-30T23:59:59"
                },
                {
                    startDateTime = "2023-02-01T06:05:07",
                    stopDateTime = "2023-03-30T23:59:59"
                }
            }
        )

        core.update_item_dictionary_value(pin_codes_item_id, "1", {code = "1111", name = "test2", mode = "disabled"})
        core.update_item_dictionary_value(pin_codes_item_id, "2", {code = "2222", name = "test2"})
        core.update_item_dictionary_value(
            daily_user_code_intervals_item_id,
            "2",
            {
                {
                    startDateTime = "2021-03-01T06:05:07",
                    stopDateTime = "2022-03-30T23:59:59"
                }
            }
        )

        fixture.set_local_time("2021-02-05T01:01:01")

        process()

        fixture.check_event_table({
            year = 2021, mounth = 3, day = 1,
            hour = 6, minute = 5, second = 7
        })

        fixture.check_timer(true, 82740000)

        check_restriction_seted({{1, true}, {2, false}})
    end

    fixture.remove_device(9000, params.manufacturer_id, params.product_type, params.product_id)
end

tests.run_with_params(
    test_impl,
    {
        {
            value = {
                manufacturer_id = 297,
                product_type = 4,
                product_id = 0
            },
            description = "test with YRD210NR605"
        },
        {
            value = {
                manufacturer_id = 265,
                product_type = 1,
                product_id = 0
            },
            description = "test with YRD210"
        },
        {
            value = {
                manufacturer_id = 297,
                product_type = 4,
                product_id = 43520
            },
            description = "test with YRD216"
        },
        {
            value = {
                manufacturer_id = 297,
                product_type = 4,
                product_id = 521
            },
            description = "test with Unknown Yale door lock"
        },
        {
            value = {
                manufacturer_id = 297,
                product_type = 3,
                product_id = 1033
            },
            description = "test with YRL210 Yale door lock"
        },
        {
            value = {
                manufacturer_id = 297,
                product_type = 2,
                product_id = 0
            },
            description = "test with YRD220-ZW Yale door lock"
        },
        {
            value = {
                manufacturer_id = 297,
                product_type = 3,
                product_id = 0
            },
            description = "test with YRL210 Yale door lock"
        },
        {
            value = {
                manufacturer_id = 297,
                product_type = 1,
                product_id = 1033
            },
            description = "test with YRL210 Yale door lock"
        },
        {
            value = {
                manufacturer_id = 297,
                product_type = 33034,
                product_id = 56577
            },
            description = "test with NF-YRD612 PBDB Yale door lock"
        }
    }
)
