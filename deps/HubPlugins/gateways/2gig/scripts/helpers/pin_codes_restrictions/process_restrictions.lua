local core = require("core")
local timer = assert(require("timer"), "Failed to load timer functionality")
local time_parser = loadfile("HUB:2gig/scripts/helpers/pin_codes_restrictions/parse_time_string")
local less_or_equal = loadfile("HUB:2gig/scripts/helpers/pin_codes_restrictions/less_or_equal")
local time_is_in_interval = loadfile("HUB:2gig/scripts/helpers/pin_codes_restrictions/time_is_in_interval")
local get_duration_to_next_event = loadfile("HUB:2gig/scripts/helpers/pin_codes_restrictions/get_duration_to_next_event")

local current_time = nil
local next_event = nil

local function ensure_next_event(restriction_action_time)
    if not current_time then
        current_time = time_parser(core.get_local_time())
    end

    if less_or_equal(current_time, restriction_action_time) then
        if not next_event or less_or_equal(restriction_action_time, next_event) then
            next_event = restriction_action_time
        end
    end
end

local function items_reactor(user_codes_item, week_user_code_restriction_item, day_user_code_restriction_item)
    for pin_code_id, pin_code_value in pairs(user_codes_item.value) do
        local old_mode = pin_code_value.mode or "enabled"
        local new_mode = nil
        
        -- process week intervals
        if core.is_inside_week_interval and core.get_next_week_event then
            if week_user_code_restriction_item.value and week_user_code_restriction_item.value[pin_code_id] then
                local week_restrictions = week_user_code_restriction_item.value[pin_code_id]
                for _, restriction in ipairs(week_restrictions) do
                    if new_mode ~= "enabled" then
                        new_mode = core.is_inside_week_interval(restriction.weekDays, restriction.startTime, restriction.stopTime) and "enabled" or "disabled"
                    end
                    
                    local nearest_start_time = core.get_next_week_event(restriction.weekDays, restriction.startTime)
                    local nearest_end_time = core.get_next_week_event(restriction.weekDays, restriction.stopTime)
                    
                    if nearest_start_time then
                        ensure_next_event(time_parser(nearest_start_time))
                    end
                    if nearest_end_time then
                        ensure_next_event(time_parser(nearest_end_time))
                    end
                end
            end
        end
        
        -- process day intervals
        if day_user_code_restriction_item.value and day_user_code_restriction_item.value[pin_code_id] then
             local day_restrictions = day_user_code_restriction_item.value[pin_code_id]
             for _, restriction in ipairs(day_restrictions) do
                local start_time = time_parser(restriction.startDateTime)
                local end_time = time_parser(restriction.stopDateTime)
                
                if new_mode ~= "enabled" then
                    new_mode = time_is_in_interval(start_time, end_time) and "enabled" or "disabled"
                end
                
                ensure_next_event(start_time)
                ensure_next_event(end_time)
             end
        end
        
        if new_mode and new_mode ~= old_mode then
            core.update_item_dictionary_value(
                user_codes_item.id, 
                pin_code_id, 
                {
                    code = pin_code_value.code,
                    name = pin_code_value.name,
                    mode = new_mode
                }
            )
        end
    end
end

loadfile("HUB:2gig/scripts/helpers/pin_codes_restrictions/for_each_pin_codes_restrictions_items")(items_reactor)

local storage_utils = require("HUB:2gig/scripts/helpers/storage_utils")
if next_event then
    storage_utils.set_nearest_restriction_event_time(next_event)
    if timer.exists("2gig_pin_codes_restrictions_timer") then
        timer.cancel("2gig_pin_codes_restrictions_timer")
    end
    
    timer.set_timeout_with_id(
        get_duration_to_next_event(next_event), 
        "2gig_pin_codes_restrictions_timer", 
        "HUB:2gig/scripts/helpers/pin_codes_restrictions/timer/handler", 
        {}
    )
else
    storage_utils.clear_nearest_restriction_event_time()
    timer.cancel("2gig_pin_codes_restrictions_timer")
end
