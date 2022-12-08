local core = require("core")
local storage_utils = require("HUB:2gig/scripts/helpers/storage_utils")
local time_parser = loadfile("HUB:2gig/scripts/helpers/pin_codes_restrictions/parse_time_string")
local less_or_equal = loadfile("HUB:2gig/scripts/helpers/pin_codes_restrictions/less_or_equal")

local restriction_event_time = storage_utils.find_nearest_restriction_event_time()

if not restriction_event_time then
    print("No next pin codes restriction event expected, ignoring timer")
    return 
end

local current_time = time_parser(core.get_local_time())

if less_or_equal(restriction_event_time, current_time) then
    loadfile("HUB:2gig/scripts/helpers/pin_codes_restrictions/process_restrictions")()
end
