local self, item_id, descriptor, pin_code_id, mode = ...

local core = require("core")

local item = assert(core.get_item(item_id))
local pin_code_info = assert(item.value[pin_code_id])
local old_mode = pin_code_info.mode or "enabled"

if old_mode ~= mode then
    print("Changing " .. pin_code_id .. " pin code status to " .. mode)

    local zwave = require("zwave")

    local value
    if mode == "disabled" then
        value = {
            weekDays = { "monday" },
            startHour = 1,
            startMinute = 1,
            durationHour = 0,
            durationMinute = 0
        }
    end

    zwave.set_daily_repeating_entry_lock_schedule(
        descriptor.node_id,
        descriptor.channel_id,
        tonumber(pin_code_id),
        1,
        value
    )

    self:_super_call("generic_doorlock", "update_active_user_code", item_id, tonumber(pin_code_id), pin_code_info.code, mode)
end
