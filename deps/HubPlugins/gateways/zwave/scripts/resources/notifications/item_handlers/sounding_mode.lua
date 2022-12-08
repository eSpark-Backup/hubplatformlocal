local sounding_mode_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/sounding_mode_event")

return setmetatable({
        can_be_reset_by_idle_event = true,
        value_after_reset = sounding_mode_enum.audible,
        get_enum_values = function (event_type) return { sounding_mode_enum.silent } end,
        get_value = function (event_type, notification_value) return sounding_mode_enum.silent end,
    },
    require("HUB:zwave/scripts/resources/notifications/item_handlers/base_token_item_handler_mt"))