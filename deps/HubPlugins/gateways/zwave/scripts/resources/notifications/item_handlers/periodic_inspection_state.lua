return {
    can_be_reset_by_idle_event = true,
    value_after_reset = "inspection_not_required",
    get_enum_values = function (event_type)
        assert(event_type == "maintenance_required_planned_periodic_inspection")
        return { event_type }
    end,
    get_value = function (event_type, notification_value) return "inspection_required" end,
}
