return {
    id = "reachable_check_policy",
    label = {
        text = "Device reachable check timeouts policy",
        lang_tag = "gateway_setting_reachable_check_policy"
    },
    status = "synced",
    value_type = "token",
    enum = {
        default = { text = "Default", lang_tag = "gateway_setting_reachable_check_policy_default" },
        use_fixed_interval = { text = "Use only fixed interval", lang_tag = "gateway_setting_reachable_check_policy_use_fixed_interval" }        
    },
    value = "default",
    value_default = "default",
    has_setter = true
}
