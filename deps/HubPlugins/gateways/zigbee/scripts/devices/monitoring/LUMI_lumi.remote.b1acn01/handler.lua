local handler = hubplugins.weak_loadfile("HUB:zigbee/scripts/devices/monitoring/default/handler")()

local button_actions = hubplugins.weak_require("HUB:zigbee/scripts/resources/enums/button_actions")
local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")

local to_item_value = {
    ["1"]   = button_actions.press_1_time,
    ["255"] = button_actions.released,
    ["0"]   = button_actions.held_down,
    ["2"]   = button_actions.press_2_times
}

local base_attr_updated = handler.attribute_value_updated
function handler.attribute_value_updated(self, node, endpoint_id, cluster_name, attribute)
    if cluster_name == "multistate_input_basic" and attribute.name == "PresentValue" then
        local item_id = handler:find_item_id(item_names.button_state, node.id, endpoint_id, cluster_name, attribute.name)
        if not item_id then
            print("Item id for updated attribute not found")
            return
        end

        local value = to_item_value[attribute.formatted_value]
        if not value then
            print("Unsupported value: ", attribute.formatted_value)
            return
        end

        handler:update_value_in_core(item_id, { button_number = 1, button_state = value })

        if value ~= button_actions.held_down then
            handler:update_value_in_core(item_id, { button_number = 1, button_state = button_actions.idle })
        end
    else
        base_attr_updated(self, node, endpoint_id, cluster_name, attribute)
    end
end

return handler
