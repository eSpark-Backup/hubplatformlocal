local start_time, end_time = ...

local core = require("core")
local time_parser = loadfile("HUB:2gig/scripts/helpers/pin_codes_restrictions/parse_time_string")
local less_or_equal = loadfile("HUB:2gig/scripts/helpers/pin_codes_restrictions/less_or_equal")

local current_time = time_parser(core.get_local_time())

return less_or_equal(start_time, current_time) and less_or_equal(current_time, end_time)
