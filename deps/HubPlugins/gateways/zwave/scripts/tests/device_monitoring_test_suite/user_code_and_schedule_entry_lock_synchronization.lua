local tests = require("tests")
local zwave = require("zwave")
local core = require("core")
local events_handler = assert(loadfile("HUB:zwave/scripts/events/events_handling"))
local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local item_names = require("HUB:zwave/scripts/resources/item_names")
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

zwave.request_node_values = tests.create_mock()

local function send_user_code(user_status_id, user_id, supported_users_count, code)
    events_handler( {
        event = "user_code_updated",
        node_id = 9000,
        class_id = class_ids.user_codes,
        channel_id = 0,
        
        user_code_ranges = {
            {
                user_id = user_id,
                user_status = user_status_id,
                user_code = code
            }
        },
        user_polling_status = "idle",
        supported_users_count = supported_users_count or 250,
    } )
end

-- week_day_slots and year_day_slots
do
    local channel_0 = {
        id = 0,
        classes = {
            { id = class_ids.user_codes, supported_users_count = 250 },
            { 
                id = class_ids.schedule_entry_lock,
                week_day_slots = 10,
                year_day_slots = 2,
                daily_repeating_slots = 0
            }
        }
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

    -- prepare zwave
    zwave.add_node( node )

    -- add node
    events_handler( {
        event = "node_added",
        node_id = 9000
    } )

    local user_codes_item_id = storage:call("find_item_id", item_names.user_codes, 9000, class_ids.user_codes, 0)
    local daily_intervals_item_id = storage:call("find_item_id", item_names.daily_user_code_intervals, 9000, class_ids.schedule_entry_lock, 0)
    local weekly_intervals_item_id = storage:call("find_item_id", item_names.weekly_user_code_intervals, 9000, class_ids.schedule_entry_lock, 0, "week_day_per_slot")

    local user_code_daily_intervals_modifier = hubplugins.object(
                                            "sequent_requests_sender/user_code_intervals", 
                                            "HUB:zwave/scripts/utils",
                                            daily_intervals_item_id, 
                                            9000, 
                                            0,
                                            item_names.daily_user_code_intervals)

    local user_code_weekly_intervals_modifier = hubplugins.object(
                                            "sequent_requests_sender/user_code_intervals", 
                                            "HUB:zwave/scripts/utils",
                                            weekly_intervals_item_id, 
                                            9000, 
                                            0,
                                            "week_day_per_slot")

    -- add initial requests for convinient comparizon
    user_code_daily_intervals_modifier:call("add_requests", { "100" })
    user_code_weekly_intervals_modifier:call("add_requests", { "100" })

    local function send_user_codes()
        send_user_code("enabled", 3, 250, "3333")
        send_user_code("enabled", 5, 250, "5555")
        send_user_code("enabled", 7, 250, "7777")
    end

    local function send_schedules()
        events_handler( {
            event = "year_day_entry_lock_schedule_updated",
            node_id = "9000",
            channel_id = 0,
            class_id = class_ids.schedule_entry_lock,
            user_id = 3,
            slot_id = 1,
            value = {
                startYear = 2020,
                startMonth = 3,
                startDay = 1,
                startHour = 10,
                startMinute = 20,
                stopYear = 2020,
                stopMonth = 3,
                stopDay = 1,
                stopHour = 10,
                stopMinute = 50,
            }
        } )

        events_handler( {
            event = "week_day_entry_lock_schedule_updated",
            node_id = "9000",
            channel_id = 0,
            class_id = class_ids.schedule_entry_lock,
            user_id = 3,
            slot_id = 1,
            value = {
                weekDay = "monday",
                startHour = 8,
                startMinute = 50,
                stopHour = 13,
                stopMinute = 1
            }
        } )

        events_handler( {
            event = "week_day_entry_lock_schedule_updated",
            node_id = "9000",
            channel_id = 0,
            class_id = class_ids.schedule_entry_lock,
            user_id = 5,
            slot_id = 2,
            value = {
                weekDay = "monday",
                startHour = 10,
                startMinute = 50,
                stopHour = 13,
                stopMinute = 1
            }
        } )
    end

    -- don't clear intervals if no actual intervals
    do
        send_user_codes()

        tests.assert_eq(user_code_daily_intervals_modifier:call("get_requests"), {})
        tests.assert_eq(user_code_weekly_intervals_modifier:call("get_requests"), {})

        send_user_code("available", 3)
        tests.assert_eq(user_code_daily_intervals_modifier:call("get_requests"), {})
        tests.assert_eq(user_code_weekly_intervals_modifier:call("get_requests"), {})

        send_user_code("available", 4)
        tests.assert_eq(user_code_daily_intervals_modifier:call("get_requests"), {})
        tests.assert_eq(user_code_weekly_intervals_modifier:call("get_requests"), {})

        send_user_code("available", 0)
        tests.assert_eq(user_code_daily_intervals_modifier:call("get_requests"), {})
        tests.assert_eq(user_code_weekly_intervals_modifier:call("get_requests"), {})
    end

    -- all user codes removed
    do
        send_user_codes()
        send_schedules()

        send_user_code("available", 0)
        tests.assert_eq(user_code_daily_intervals_modifier:call("get_requests"), { "3" })
        tests.assert_eq(user_code_weekly_intervals_modifier:call("get_requests"), { "3", "5" })

        user_code_daily_intervals_modifier:call("remove_all_requests")
        user_code_weekly_intervals_modifier:call("remove_all_requests")
    end

    -- some user codes removed
    do
        send_user_codes()
        send_schedules()

        send_user_code("available", 3)
        tests.assert_eq(user_code_daily_intervals_modifier:call("get_requests"), { "3" })
        tests.assert_eq(user_code_weekly_intervals_modifier:call("get_requests"), { "3" })

        send_user_code("available", 5)
        tests.assert_eq(user_code_daily_intervals_modifier:call("get_requests"), { "3" })
        tests.assert_eq(user_code_weekly_intervals_modifier:call("get_requests"), { "3", "5" })

        send_user_code("available", 7)
        tests.assert_eq(user_code_daily_intervals_modifier:call("get_requests"), { "3" })
        tests.assert_eq(user_code_weekly_intervals_modifier:call("get_requests"), { "3", "5" })
    end
     
    events_handler( {
        event = "node_removed",
        node_id = 9000,
        manufacturer_id = 0,
        product_type = 1,
        product_id = 2,
    } )
end

-- daily_repeating_slots
do
    local channel_0 = {
        id = 0,
        classes = {
            { id = class_ids.user_codes, supported_users_count = 250 },
            { 
                id = class_ids.schedule_entry_lock,
                week_day_slots = 0,
                year_day_slots = 0,
                daily_repeating_slots = 5
            }
        }
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

    -- prepare zwave
    zwave.add_node( node )

    -- add node
    events_handler( {
        event = "node_added",
        node_id = 9000
    } )

    local user_codes_item_id = storage:call("find_item_id", item_names.user_codes, 9000, class_ids.user_codes, 0)
    local weekly_intervals_item_id = storage:call("find_item_id", item_names.weekly_user_code_intervals, 9000, class_ids.schedule_entry_lock, 0)

    local user_code_weekly_intervals_modifier = hubplugins.object(
                                            "sequent_requests_sender/user_code_intervals", 
                                            "HUB:zwave/scripts/utils",
                                            weekly_intervals_item_id, 
                                            9000, 
                                            0,
                                            "week_day_per_slot")

    -- add initial requests for convinient comparizon
    user_code_weekly_intervals_modifier:call("add_requests", { "100" })

    local function send_user_codes()
        send_user_code("enabled", 3, 250, "3333")
        send_user_code("enabled", 5, 250, "5555")
        send_user_code("enabled", 7, 250, "7777")
    end

    local function send_schedules()
        events_handler( {
            event = "daily_repeating_entry_lock_schedule_updated",
            node_id = "9000",
            channel_id = 0,
            class_id = class_ids.schedule_entry_lock,
            user_id = 3,
            slot_id = 1,
            value = {
                weekDays = { "monday", "wednesday" },
                startHour = 11,
                startMinute = 50,
                durationHour = 1,
                durationMinute = 11
            }
        } )

        events_handler( {
            event = "daily_repeating_entry_lock_schedule_updated",
            node_id = "9000",
            channel_id = 0,
            class_id = class_ids.schedule_entry_lock,
            user_id = 5,
            slot_id = 2,
            value = {
                weekDays = { "monday" },
                startHour = 11,
                startMinute = 50,
                durationHour = 1,
                durationMinute = 11
            }
        } )
    end

    -- all user codes removed
    do
        send_user_codes()
        send_schedules()

        send_user_code("available", 0)
        tests.assert_eq(user_code_weekly_intervals_modifier:call("get_requests"), { "3", "5" })

        user_code_weekly_intervals_modifier:call("remove_all_requests")
    end

    -- some user codes removed
    do
        send_user_codes()
        send_schedules()

        send_user_code("available", 3)
        tests.assert_eq(user_code_weekly_intervals_modifier:call("get_requests"), { "3" })

        send_user_code("available", 5)
        tests.assert_eq(user_code_weekly_intervals_modifier:call("get_requests"), { "3", "5" })

        send_user_code("available", 7)
        tests.assert_eq(user_code_weekly_intervals_modifier:call("get_requests"), { "3", "5" })
    end
     
    events_handler( {
        event = "node_removed",
        node_id = 9000,
        manufacturer_id = 0,
        product_type = 1,
        product_id = 2,
    } )
end