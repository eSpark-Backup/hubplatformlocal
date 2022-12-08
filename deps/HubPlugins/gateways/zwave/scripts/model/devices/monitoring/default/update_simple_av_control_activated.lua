local self, node_id, channel_id, class_id, key_attr, commands = ...

local timer = require("timer")
local item_names = require("HUB:zwave/scripts/resources/item_names")
local button_states = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/button_actions")

local device_info = self:call("get_device_info")

if not device_info.simple_av_control_actions then return end

for _, command in ipairs(commands) do
    local button_number
    for _, info in ipairs(device_info.simple_av_control_actions) do
        if info.command_number == command then
            button_number = info.button_number
        end
    end

    if button_number then
        local item_id = self:call("find_item_id", item_names.button_state, node_id, channel_id, class_id, button_number)

        if item_id then
            if key_attr == button_states.press_1_time then
                timer.set_timeout(
                    500,
                    "HUB:zwave/scripts/timer_handlers/timeout/update_simple_av_control_1_press",
                    {item_id = item_id, node_id = node_id})
            else
                self:call("update_value_in_core",
                    item_names.button_state,
                    item_id,
                    { button_number = button_number, button_state = key_attr }
                )

                if key_attr ~= button_states.held_down then
                    self:call("update_value_in_core",
                        item_names.button_state,
                        item_id,
                        { button_number = button_number, button_state = button_states.idle }
                    )
                end
            end
        end
    end
end
