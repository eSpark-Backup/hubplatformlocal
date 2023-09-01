local cls_ias_ace = hubplugins.weak_loadfile("HUB:zigbee/scripts/devices/clusters/base")()

local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")

function cls_ias_ace.process_cluster(self, inclusion_context)
    if inclusion_context.cluster.data_holder ~= "client" then return end

    self:process_pin_codes(inclusion_context)
    self:process_emergency_state(inclusion_context)
end

function cls_ias_ace.get_elements_max_number(self, dev_info)
    if not dev_info.first_user_code and not dev_info.end_user_code then
        return 100
    end

    local first_user_code = tonumber(dev_info.first_user_code or 1)
    local end_user_code = tonumber(dev_info.end_user_code or 100)

    return end_user_code - first_user_code + 1
end

function cls_ias_ace.process_emergency_state(self, inclusion_context)
    local emergency_state_enum = hubplugins.weak_loadfile("HUB:zigbee/scripts/resources/enums/emergency_state")()
    local item = self:create_item(inclusion_context, "", item_names.emergency_state)

    local add_item = false
    if self:find_command("Panic", inclusion_context.cluster) then
        table.insert(item.enum, emergency_state_enum.panic)
        add_item = true
    end

    if self:find_command("Emergency", inclusion_context.cluster) then
        table.insert(item.enum, emergency_state_enum.emergency)
        add_item = true
    end

    if add_item then
        self:add_items_and_settings_to_child_device(inclusion_context, item.name, {item})
    end
end

function cls_ias_ace.process_pin_codes(self, inclusion_context)
    if self:find_command("Arm", inclusion_context.cluster) then
        local items = {
            self:create_item(inclusion_context, "", item_names.user_codes),
            self:create_item(inclusion_context, "", item_names.user_code_action),
            self:create_item(inclusion_context, "", item_names.zones_status)
        }

        local device_node_data = inclusion_context.tree_model:call("get_data", inclusion_context.current_processing_node)
        items[1].elements_max_number = assert(self:get_elements_max_number(device_node_data.info), "No user code max number.")
        items[1].user_code_restriction = assert(device_node_data.info.user_code_restriction, "No user code restriction specified")

        self:add_items_and_settings(inclusion_context, items)
    end
end

return cls_ias_ace
