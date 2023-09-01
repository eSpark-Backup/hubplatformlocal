local item_id, pin_code_id, mode, delay = ...

local timer = assert(require("timer"), "Failed to load timer functionality")

timer.set_timeout(
    delay * 1000,
    "HUB:zwave/scripts/helpers/virtual_pin_codes_restrictions/timer/change_pin_code_mode_handler",
    {
        item_id = item_id,
        pin_code_id = pin_code_id,
        mode = mode
    }
)
