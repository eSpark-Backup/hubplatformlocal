local self, inclusion_context, device_node_data, cc = ...

local function add_setting(setting_tag, capability)
    self:call("add_door_lock_setting", inclusion_context, device_node_data, setting_tag, capability)
end

if cc.capabilities then
    if cc.capabilities.timed_operations_supported then
        add_setting("timed_operations_enabled")
        add_setting("lock_timeout")
    end

    if cc.capabilities.auto_relock_supported then
        add_setting("auto_relock_timeout")
    end

    if cc.capabilities.hold_and_release_supported then
        add_setting("hold_and_release_timeout")
    end

    if cc.capabilities.twist_assist_supported then
        add_setting("twist_assist_enabled")
    end

    if cc.capabilities.block_to_block_supported then
        add_setting("block_to_block_enabled")
    end

    if cc.capabilities.inside_handle_ids and cc.capabilities.inside_handle_ids[1] then
        add_setting("enabled_inside_handle_ids", cc.capabilities.inside_handle_ids)
    end

    if cc.capabilities.outside_handle_ids and cc.capabilities.outside_handle_ids[1] then
        add_setting("enabled_outside_handle_ids", cc.capabilities.outside_handle_ids)
    end

    if cc.capabilities.bolt_supported then
        -- TODO:
    end
else
    add_setting("timed_operations_enabled")
    add_setting("lock_timeout")
    add_setting("enabled_inside_handle_ids", { 1, 2, 3, 4 })
    add_setting("enabled_outside_handle_ids", { 1, 2, 3, 4 })
end
