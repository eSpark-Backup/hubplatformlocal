local handler = {}

local core = assert(require "core", "Fail to load core functionality")
local storage = hubplugins.object("storage_helper", "HUB:zigbee/scripts/utils")
local zigbee = require("zigbee")

function handler.attribute_value_updated(self, node, endpoint_id, cluster_name, attribute)
    local device_id = storage:call("find_device_id", node.id)
    if device_id then
        local attribute_handle = hubplugins.weak_loadfile("HUB:zigbee/scripts/devices/monitoring/attribute_value_updated/" .. cluster_name)
        if attribute_handle then
            attribute_handle(self, node, device_id, endpoint_id, cluster_name, attribute)
        else
            print( "No monitoring functionality for " .. cluster_name .. " cluster" )
        end
    end
end

function handler.mfg_specific_attribute_value_updated(self, node, endpoint_id, cluster_id, attribute)
    print("No mfg specific attributes monitoring supported")
end

function handler.unknown_attribute_value_updated(
    self, node, endpoint_id, cluster_name, cluster_id, attribute_name, attribute_id, value)
    print("No unknown attributes monitoring supported")
end

function handler.arm_command_received(self, node, endpoint_id, cluster_name, arm_mode, pin_code)
    hubplugins.weak_loadfile("HUB:zigbee/scripts/devices/monitoring/arm_command_received")(
        self,
        node,
        endpoint_id,
        cluster_name,
        arm_mode,
        pin_code
    )
end

function handler.door_lock_programming_event_notification_received(
    self, node, endpoint_id, cluster_name, event_source, event_name, user_id, pin_code, user_type, user_status
)
    hubplugins.weak_loadfile("HUB:zigbee/scripts/devices/monitoring/door_lock_programming_event_notification_received")(
        self,
        node,
        endpoint_id,
        cluster_name,
        event_source,
        event_name,
        user_id,
        pin_code,
        user_type,
        user_status
    )
end

function handler.door_lock_operating_event_notification_received(
    self, node, endpoint_id, cluster_name, event_source, event_name, user_id, pin_code
)
    hubplugins.weak_loadfile("HUB:zigbee/scripts/devices/monitoring/door_lock_operating_event_notification_received")(
        self,
        node,
        endpoint_id,
        cluster_name,
        event_source,
        event_name,
        user_id,
        pin_code
    )
end

function handler.get_user_response_command_received(self, node, endpoint_id, cluster_name, user_type, user_status, user_id, pin_code)
    hubplugins.weak_loadfile("HUB:zigbee/scripts/devices/monitoring/get_user_response_command_received")(
        self,
        node,
        endpoint_id,
        cluster_name,
        user_type,
        user_status,
        user_id,
        pin_code
    )
end

function handler.alert_received(self, node, alert_id, category, cluster_name, command_id, command_name, endpoint_id, manufacturer_specific_bits, presence_recovery)
    print("Event \"".. command_name .."\" was received, but it does not implement for this device!")
end

function handler.empty_alerts_list_received(self, node, cluster_name, command_id, command_name, endpoint_id)
    print("Event \"".. command_name .."\" was received, but it does not implement for this device!")
end

function handler.user_codes_request_progress(self, node, endpoint_id, progress)
    local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")

    local scan_progress_item_id = handler:find_item_id(
        item_names.user_codes_scan_progress,
        node.id,
        endpoint_id,
        "door_lock",
        ""
    )
    if not scan_progress_item_id then return end

    self:update_value_in_core(scan_progress_item_id, progress)
end

function handler.get_user_code_with_name(self, pin_code, user_id_string, pin_codes_item, new_mode)
    local user_code = pin_codes_item.value[user_id_string]
    if user_code then
        user_code.code = pin_code
        user_code.mode = new_mode
    else
        user_code = {
            code = pin_code,
            name = storage:call(
                "get_table_value",
                storage:call("get_usernames_tablename", pin_codes_item.id),
                user_id_string
                ) or user_id_string,
            mode = new_mode
        }
        storage:call(
            "remove_table_value",
            storage:call("get_usernames_tablename", pin_codes_item.id),
            user_id_string
        )
    end
    return user_code
end

function handler.simple_command_received(self, node, endpoint_id, cluster_name, command_id, command_name)
    local cluster_handler = hubplugins.weak_loadfile("HUB:zigbee/scripts/devices/monitoring/simple_command_received/" .. cluster_name)
    if not cluster_handler then
        print("No handler for simple command, cluster: " .. cluster_name)
        return
    end

    cluster_handler(self, node, endpoint_id, cluster_name, command_id, command_name)
end

function handler.update_reachable_state(self, node_id, value)
    local device_id = storage:call("find_device_id", node_id)
    if not device_id then return end

    local function callback(device)
        core.update_reachable_state(device.id, value)
    end

    hubplugins.weak_loadfile("HUB:zigbee/scripts/helpers/for_each_device")(device_id, callback, core)
end

function handler.house_mode_switched(self, item_id, descriptor, from, to, status, switch_delay)
    hubplugins.weak_loadfile("HUB:zigbee/scripts/devices/monitoring/house_mode_switched")(
        self,
        item_id,
        descriptor,
        from,
        to,
        status,
        switch_delay
    )
end

function handler.house_mode_alarmed(self, item_id, descriptor, house_mode_id)
    hubplugins.weak_loadfile("HUB:zigbee/scripts/devices/monitoring/house_mode_alarmed")(
        self,
        item_id,
        descriptor,
        house_mode_id
    )
end

function handler.request_node_values(self, node_id)
    local device_id = storage:call("find_device_id", node_id)
    if not device_id then return end

    hubplugins.weak_loadfile("HUB:zigbee/scripts/helpers/request_device_values")(device_id)
end

function handler.set_item_value(self, item_id, descriptor, value)
    local command_to_send = self:find_command_to_send( descriptor.cluster_name, descriptor.item_name, value )

    if command_to_send then
        zigbee.send_command(
            descriptor.node_id,
            descriptor.endpoint_id,
            descriptor.cluster_name,
            command_to_send.command_name,
            command_to_send.command_params
        )
    else
        self:try_set_attribute_value(item_id, descriptor, value)
    end
end

function handler.add_item_dictionary_value(self, item_id, descriptor, value_type, value)
    self:set_item_dictionary_value(item_id, descriptor, nil, value_type, value)
end

function handler.set_item_dictionary_value(self, item_id, descriptor, key, value_type, value)
    local cluster_handler = hubplugins.weak_loadfile("HUB:zigbee/scripts/devices/monitoring/set_item_dictionary_value/" .. descriptor.cluster_name)
    if not cluster_handler then
        print("No handler for dictionary value setting, cluster: " .. descriptor.cluster_name)
        return
    end

    cluster_handler(self, item_id, descriptor, key, value_type, value)
end

function handler.remove_item_dictionary_value(self, item_id, descriptor, key)
    local cluster_handler = hubplugins.weak_loadfile("HUB:zigbee/scripts/devices/monitoring/remove_item_dictionary_value/" .. descriptor.cluster_name)
    if not cluster_handler then
        print("No handler for dictionary value removing, cluster: " .. descriptor.cluster_name)
        return
    end

    cluster_handler(self, item_id, descriptor, key)
end

function handler.reset_setting_value(self, setting_id, descriptor)
    local cluster_handler = hubplugins.weak_loadfile("HUB:zigbee/scripts/devices/monitoring/reset_setting_value/" .. descriptor.cluster_name)
    if not cluster_handler then
        print("No handler for setting value resetting, cluster: " .. descriptor.cluster_name)
        return
    end

    cluster_handler(self, setting_id, descriptor)
end

function handler.set_setting_value(self, setting_id, descriptor, value)
    local cluster_handler = hubplugins.weak_loadfile("HUB:zigbee/scripts/devices/monitoring/set_setting_value/" .. descriptor.cluster_name)
    if not cluster_handler then
        print("No handler for setting value setting, cluster: " .. descriptor.cluster_name)
        return
    end

    cluster_handler(self, setting_id, descriptor, value)
end

function handler.stop_fast_poll(self, node)
    for _,endpoint in ipairs(node.endpoints) do
        for _,cluster in ipairs(endpoint.clusters) do
            if cluster.name == "poll_control" then
                zigbee.send_command( node.id, endpoint.id, "poll_control", "FastPollStop" )
            end
        end
    end
end

function handler.find_command_to_send(self, cluster_name, item_name, value)
    local cluster_commands_mappings = hubplugins.weak_loadfile("HUB:zigbee/scripts/resources/commands_mappings/" .. cluster_name)
    if not cluster_commands_mappings then
        print("No commands mappings for cluster: " .. cluster_name)
        return
    end

    local command_name = cluster_commands_mappings().get_command_name(item_name, value)
    local command_params = cluster_commands_mappings().get_command_params(item_name, value)
    if not command_name then
        print("No command name found for item: " .. item_name .. ", cluster: " .. cluster_name)
        return
    end

    return {command_name = command_name, command_params = command_params}
end

function handler.try_set_attribute_value(self, item_id, descriptor, value)
    local cluster_handler = hubplugins.weak_loadfile("HUB:zigbee/scripts/devices/monitoring/set_item_value/" .. descriptor.cluster_name)
    if not cluster_handler then
        print("No handler for attribute value setting, cluster: " .. descriptor.cluster_name)
        return
    end

    cluster_handler(self, item_id, descriptor, value)
end

function handler.request_item_value_with_timeout(self, node_id, endpoint_id, cluster_name, attribute_name, timeout)
    local timer = assert(require("timer"), "Fail to load the 'timer' functionality")
    local timer_args = {
        node_id = node_id,
        endpoint_id = endpoint_id,
        cluster_name = cluster_name,
        attribute_name = attribute_name
    }
    timeout = timeout or 3000
    timer.set_timeout(timeout, "HUB:zigbee/scripts/timer_handlers/timeout/request_attribute_value", timer_args)
end

function handler.find_attribute(self, node, endpoint_id, cluster_name, attribute_name)
    return hubplugins.weak_loadfile("HUB:zigbee/scripts/helpers/find_attribute")(
        node,
        endpoint_id,
        cluster_name,
        attribute_name
    )
end

function handler.find_attribute_by_ids(self, node, endpoint_id, cluster_id, attribute_id)
    for _,endpoint in ipairs(node.endpoints) do
        if endpoint.id == endpoint_id then
            for _,cluster in ipairs(endpoint.clusters) do
                if cluster.id == cluster_id then
                    for _,attribute in ipairs(cluster.attributes) do
                        if attribute.id == attribute_id then return attribute end
                    end
                end
            end
        end
    end
end

function handler.update_value_in_core(self, item_id, value)
    core.update_item_value(item_id, value)
    self:update_security_item(item_id, value)
end

function handler.update_value_in_core_with_timeout(self, node_id, item_id, value, timeout)
    local timer = assert(require("timer"), "Fail to load the 'timer' functionality")
    local timer_args = {
        node_id = node_id,
        item_id = item_id,
        item_value = value
    }

    timer.set_timeout(timeout, "HUB:zigbee/scripts/timer_handlers/timeout/set_item_value", timer_args)
end

function handler.update_setting_value_in_core(self, setting_id, value)
    core.set_setting_value(setting_id, value, "synced")
end

function handler.update_value_with_min_max_in_core(self, item_id, value, min_value, max_value)
    core.update_item_value_with_min_max(item_id, value, min_value, max_value)
end

function handler.update_value_with_min_max_in_core_with_calibration(self, item_id, value, min_value, max_value, calibration)
    value.value = value.value + calibration
    if value.value < min_value.value then
        value.value = min_value.value
    elseif value.value > max_value.value then
        value.value = max_value.value
    end

    self:update_value_with_min_max_in_core(item_id, value, min_value, max_value)
end

function handler.update_security_item( self, item_id, value )
    local security_items = hubplugins.weak_loadfile("HUB:zigbee/scripts/resources/security_items")()
    local security_tag_helper = hubplugins.weak_loadfile("HUB:zigbee/scripts/helpers/security_tag_helper")()

    local descriptor = assert( storage:call("find_item_descriptor", item_id), "No item descriptor")
    if security_items[ descriptor.item_name ] then
        descriptor.tag = security_tag_helper.add_security_tag(descriptor.tag)
        local security_item_id = assert( storage:call("find_item_id", descriptor), "No security item id")
        local security_value = security_items[ descriptor.item_name ][ value ] or false
        core.update_item_value(security_item_id, security_value)
    end
end

function handler.modify_item_in_core(self, item)
    core.modify_item(item.id, item)
end

function handler.find_item_id(self, item_name, node_id, endpoint_id, cluster_name, attribute_name, tag)
    local item_id = storage:call("find_item_id", self:create_item_descriptor(item_name, node_id, endpoint_id, cluster_name, attribute_name, tag))

    if item_id then
        return item_id
    else
        print( string.format(
            "Item id not found by item descriptor: item name: %s, node id: %g, endpoint id: %g, cluster name: %s, attribute_name: %s, tag: %s",
            item_name,
            node_id,
            endpoint_id,
            cluster_name,
            attribute_name,
            tag or "" ) )
    end
end

function handler.find_item(self, item_name, node_id, endpoint_id, cluster_name, attribute_name, tag)
    local item_id = self:find_item_id(item_name, node_id, endpoint_id, cluster_name, attribute_name, tag)
    if item_id then
        return assert( core.get_item(item_id), "No item found with id: " .. item_id)
    end
end

function handler.find_setting(self, setting_name, node_id, endpoint_id, cluster_name, attribute_name)
    local descriptor = self:create_setting_descriptor(setting_name, node_id, endpoint_id, cluster_name, attribute_name)
    local setting_id = storage:call("find_setting_id", descriptor)
    if setting_id then
        return core.get_setting(setting_id)
    end
end

function handler.create_item_descriptor(self, item_name, node_id, endpoint_id, cluster_name, attribute_name, tag)
    return {
        item_name = item_name,
        node_id = node_id,
        endpoint_id = endpoint_id,
        cluster_name = cluster_name,
        attribute_name = attribute_name,
        tag = tag
    }
end

function handler.create_setting_descriptor(self, setting_name, node_id, endpoint_id, cluster_name, attribute_name)
    return {
        setting_name = setting_name,
        node_id = node_id,
        endpoint_id = endpoint_id,
        cluster_name = cluster_name,
        attribute_name = attribute_name,
    }
end

function handler.get_setpoint_dead_band(self, node, endpoint_id)
    local dead_band = 2

    local attr = self:find_attribute(node, endpoint_id, "thermostat", "MinSetpointDeadBand")
    if attr then
        dead_band = attr.formatted_value / 10
    end

    return dead_band
end

function handler.to_temperature_with_divider(self, value, divider)
    return { value = value / divider, scale = "celsius"}
end

function handler.to_temperature(self, value)
    return handler:to_temperature_with_divider(value, 100)
end

function handler.to_relative_mesurement(self, value)
    return { value = value / 100, scale = "percent"}
end

function handler.to_illuminance(self, value)
    local value_in_lux = 10 ^ ((value - 1) / 10000)
    return { value = math.floor(value_in_lux), scale = "lux" }
end

function handler.calculate_min_max_value(self, node, cluster_name, item_name, attributes_values_getter)
    local min, max = self:get_integrated_device_min_max_value(node, cluster_name, item_name)
    if not min then
        min, max = attributes_values_getter()
    end

    if not min then
        min, max = self:get_default_device_min_max_value(node, cluster_name, item_name)
    end

    assert(min and max)
    return min, max
end

function handler.get_integrated_device_min_max_value(self, node, cluster_name, item_name)
    return self:_get_device_min_max_value(node, cluster_name, item_name, "min_max_values")
end

function handler.get_default_device_min_max_value(self, node, cluster_name, item_name)
    return self:_get_device_min_max_value(node, cluster_name, item_name, "default_min_max_values")
end

function handler._get_device_min_max_value(self, node, cluster_name, item_name, info_field_name)
    local dev_info = self:get_device_info(node)
    if dev_info[info_field_name] then
        for _, man_max_value in ipairs(dev_info[info_field_name]) do
            if man_max_value.cluster_name == cluster_name and man_max_value.item_name == item_name then
                return man_max_value.min, man_max_value.max
            end
        end
    end
end

function handler.get_device_info(self, node)
    local dev_handler_id = hubplugins.weak_loadfile("HUB:zigbee/scripts/helpers/create_device_type_id")(node.manufacturer_id, node.model_id)
    return hubplugins.weak_loadfile("HUB:zigbee/scripts/helpers/get_device_info")(dev_handler_id)
end

return handler
