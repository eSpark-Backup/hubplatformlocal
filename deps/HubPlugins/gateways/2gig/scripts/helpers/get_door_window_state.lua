local serial, reed_closed, external_contact = ...

local core = require("core")
local storage_utils = require("HUB:2gig/scripts/helpers/storage_utils")
local door_window_states = require("HUB:2gig/scripts/enum/door_window_states")

local device_id = storage_utils.find_device_id(serial)
local device = core.get_device(device_id)
local device_model = device and device.info and device.info.model

local is_closed = nil

if device_model == "DW10-345" then
    local setting_id = storage_utils.find_setting_id(device_id, "dw_detection_source")
    if setting_id then
        local setting = core.get_setting(setting_id)
        if setting then
            local magnet_is_closed = not reed_closed
            local external_contact_is_closed = not external_contact

            if setting.value == "magnet_contact" then
                is_closed = magnet_is_closed
            elseif setting.value == "external_contact" then
                is_closed = external_contact_is_closed
            elseif setting.value == "mixed_optimistic" then
                is_closed = magnet_is_closed or external_contact_is_closed
            elseif setting.value == "mixed_pessimistic" then
                is_closed = magnet_is_closed and external_contact_is_closed
            end
        end
    end

    if is_closed == nil then
        is_closed = not reed_closed
    end
elseif device_model == "DW20R-345" then
    is_closed = not external_contact
else
    is_closed = reed_closed or (not external_contact )
end

return door_window_states.from_bool(is_closed)
