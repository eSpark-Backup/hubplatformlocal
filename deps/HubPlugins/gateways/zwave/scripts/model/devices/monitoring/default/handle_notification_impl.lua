local self, node_id, channel_id, class_id, notification_type, event_type, notification_value, raw_value = ...

local core = require("core")

local function reset_item(notification_mappings, item_name, opt_event_type)
    local item_data = notification_mappings.items_data[ item_name ]
    if item_data and item_data.can_be_reset_by_idle_event then
        local item_id = self:call("find_item_id", item_name, node_id, channel_id, class_id, notification_mappings.item_tag)
        if item_id then
            local item = self:call("find_item", item_name, node_id, channel_id, class_id, notification_mappings.item_tag)
            if string.match( item.value_type, "dictionary") then
                assert(item_data.dictionary_value_after_reset)
                local new_value = item_data.dictionary_value_after_reset(item.value, opt_event_type)
                for key, value in pairs(item.value) do
                    if new_value[key] then
                        core.update_item_dictionary_value(item_id, key, new_value[key])
                        new_value[key] = nil
                    else
                        core.remove_item_dictionary_value(item_id, key)
                    end
                end
                for key, value in pairs(new_value) do
                    core.add_item_dictionary_value(item_id, key, value)
                end
            else
                self:call("update_value_in_core", item_name, item_id, item_data.value_after_reset)
            end
            return true
        end
    end

    return false
end

-- main

local notification_mappings = hubplugins.weak_require("HUB:zwave/scripts/resources/notifications/" .. notification_type)
if not notification_mappings then
    print("[handler.handle_notification_impl]: notification_mappings not found for notification type " .. notification_type)
    return
end

-- reset items by idle state
if event_type == "state_idle" then
    if notification_value then
        local item_names = notification_mappings.events_to_item_names[ notification_value ]
        for _, item_name in hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/notification_item_names_iterator")(item_names) do
            assert(reset_item(notification_mappings, item_name, notification_value), "Failed to reset " .. item_name .. " item.")
        end
    else
        for item_name,_ in pairs(notification_mappings.items_data) do
            reset_item(notification_mappings, item_name)
        end
    end
else
    local item_names = notification_mappings.events_to_item_names[ event_type ]
    assert(item_names, "[handle_notification]: cannot find item names for event " .. event_type)
    for _, item_name in hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/notification_item_names_iterator")(item_names) do
        local item_data = notification_mappings.items_data[ item_name ]

        if item_data.callback then
            item_data.callback(self, node_id, channel_id, event_type, notification_value)
        end

        local value
        if item_data.get_value then
            value = item_data.get_value(event_type, notification_value)
        else
            error("Unknown value making method")
        end

        assert(value, "[handler.handle_notification_impl]: value is empty for event " .. event_type)

        local item_id = self:call("find_item_id", item_name, node_id, channel_id, class_id, notification_mappings.item_tag)

        if item_id then
            local item = self:call("find_item", item_name, node_id, channel_id, class_id, notification_mappings.item_tag)
            if string.match( item.value_type, "dictionary") then
                self:call("update_dictionary_value_in_core", item_name, item_id, value)
            else
                self:call("update_value_in_core", item_name, item_id, value)
            end

            if item_data.value_after_reset and value ~= item_data.value_after_reset then
                if item_data.should_reset_immediately then
                    self:call("update_value_in_core", item_name, item_id, item_data.value_after_reset)
                end

                if item_data.should_reset_on_timeout then
                    local timer_args = {
                        node_id = node_id,
                        item_name = item_name,
                        item_id = item_id,
                        value = item_data.value_after_reset,
                    }

                    local timer = require("timer")
                    local timer_id = "notification_value_reset_" .. item_id

                    timer.cancel(timer_id)
                    timer.set_timeout_with_id(6000, timer_id,"HUB:zwave/scripts/timer_handlers/timeout/update_value_in_core", timer_args)
                end
            end
        else
            print("Item " .. item_name .. " is not found during handling notification " .. notification_type .. "," .. event_type .. " !!!")
        end
    end
end
