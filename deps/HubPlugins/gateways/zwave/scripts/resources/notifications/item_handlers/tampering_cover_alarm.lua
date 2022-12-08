local tampering_cover_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/tampering_cover_events")

return setmetatable({
        can_be_reset_by_idle_event = true,
        value_after_reset = tampering_cover_enum.no_tampering_cover,
        get_enum_values = function (event_type) return { tampering_cover_enum.tampering_cover } end,
        get_value = function (event_type, notification_value) return tampering_cover_enum.tampering_cover end
    },
    require("HUB:zwave/scripts/resources/notifications/item_handlers/base_token_item_handler_mt"))