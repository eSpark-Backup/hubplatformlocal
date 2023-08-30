local cls_door_lock = hubplugins.weak_loadfile("HUB:zigbee/scripts/devices/clusters/base")()

function cls_door_lock.process_cluster(self, inclusion_context)
    local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")

    local items = {}

    if self:find_attribute("LockState", inclusion_context.cluster) then
        table.insert(items, self:create_item(inclusion_context, "LockState", item_names.door_lock))
    end

    if  self:find_command("SetPinCode", inclusion_context.cluster)
    and self:find_command("GetPinCode", inclusion_context.cluster)
    and self:find_command("ClearPinCode", inclusion_context.cluster) then
        local pin_codes_item = self:create_item(inclusion_context, "", item_names.user_codes)
        pin_codes_item.elements_max_number = self:get_pin_codes_max_count(inclusion_context)
        pin_codes_item.user_code_restriction = self:get_pin_code_restriction(inclusion_context)

        local scan_progress_item = self:create_item(inclusion_context, "", item_names.user_codes_scan_progress)

        table.insert(items, pin_codes_item)
        table.insert(items, scan_progress_item)
    end

    if self:find_command("OperatingEventNotification", inclusion_context.cluster) then
        table.insert(items, self:create_item(inclusion_context, "", item_names.user_lock_operation))
    end

    self:add_items_and_settings(inclusion_context, items)
end

function cls_door_lock.get_pin_codes_max_count(self, inclusion_context)
    local attr = self:find_attribute("NumberOfPINUsersSupported", inclusion_context.cluster)
    if attr then
        return attr.formatted_value
    else
        local dev_info = inclusion_context.tree_model:call("get_data", inclusion_context.current_processing_node).info

        if not dev_info.first_user_code and not dev_info.end_user_code then
            return 100
        end

        local first_user_code = tonumber(dev_info.first_user_code or 1)
        local end_user_code = tonumber(dev_info.end_user_code or 100)

        return end_user_code - first_user_code + 1
    end
end

function cls_door_lock.get_pin_code_restriction(self, inclusion_context)
    local attr_min = self:find_attribute("MinPINCodeLength", inclusion_context.cluster)
    local attr_max = self:find_attribute("MaxPINCodeLength", inclusion_context.cluster)

    if attr_min and attr_max then
        local length_param = ""

        if attr_min.formatted_value == attr_max.formatted_value then
            length_param = tostring(math.floor(attr_min.formatted_value))
        else
            length_param = tostring(math.floor(attr_min.formatted_value)) .. "," ..
                           tostring(math.floor(attr_max.formatted_value))
        end

        return "^\\d{" .. length_param .. "}$"
    end

    local dev_info = inclusion_context.tree_model:call("get_data", inclusion_context.current_processing_node).info
    return dev_info.user_code_restriction
end

return cls_door_lock
