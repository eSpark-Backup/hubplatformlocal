local serial, reed_closed, external_contact = ...

local core = require("core")
local storage_utils = require("HUB:2gig/scripts/helpers/storage_utils")
local door_window_states = require("HUB:2gig/scripts/enum/door_window_states")

local device_id = storage_utils.find_device_id(serial)
local device = core.get_device(device_id)
local device_model = device and device.info and device.info.model

local is_closed

if device_model == "DW10-345" then
    is_closed = not reed_closed
elseif device_model == "DW20R-345" then
    is_closed = not external_contact
else
    is_closed = reed_closed or (not external_contact )
end

return door_window_states.from_bool(is_closed)
