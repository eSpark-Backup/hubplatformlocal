local multistate_input_basic = hubplugins.weak_loadfile("HUB:zigbee/scripts/devices/clusters/base")()

local button_actions = hubplugins.weak_require("HUB:zigbee/scripts/resources/enums/button_actions")
local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")

function multistate_input_basic.process_cluster(self, inclusion_context)
    for _, attribute in ipairs(inclusion_context.cluster.attributes) do
        if attribute.name == "PresentValue" then
            local item = self:create_item(inclusion_context, "PresentValue", item_names.button_state)
            item.value = {
                button_number = 1,
                button_state = button_actions.idle
            }
            item.enum = {
                button_actions.idle,
                button_actions.press_1_time,
                button_actions.released,
                button_actions.held_down,
                button_actions.press_2_times
            }
            self:add_items_and_settings(
                inclusion_context,
                { item })
        end
    end
end

return multistate_input_basic
