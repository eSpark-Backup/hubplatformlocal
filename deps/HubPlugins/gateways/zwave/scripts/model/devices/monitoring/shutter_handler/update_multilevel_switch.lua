local self, node_id, channel_id, class_id, value, target_value, target_duration = ...

if value == 0 or value == 99 or ( value >= 49 and value <= 51 ) then
    local item_names = require("HUB:zwave/scripts/resources/item_names")

    local state_item_id = self:call("find_item_id", item_names.shutter_state, node_id, channel_id, class_id)
    local state_item = self:call("find_item", item_names.shutter_state, node_id, channel_id, class_id)
    assert(state_item, "No item with shutter state")
    local state_value = state_item.value
    local shutter_states_enum = require("HUB:zwave/scripts/resources/enums/shutter_states")

    if state_value == shutter_states_enum.idle then

        local command_item = self:call("find_item", item_names.shutter_command, node_id, channel_id, class_id)
        assert(command_item, "No item with shutter command")
        local command_value = command_item.value
        local shutter_commands_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/shutter_commands")

        local start_learning = false
        if command_value == shutter_commands_enum.learn_upper and value == 99 then
            self:call("update_value_in_core", state_item.name, state_item_id, shutter_states_enum.learn_upper)
            start_learning = true
        elseif command_value == shutter_commands_enum.learn_lower and value == 0 then
            self:call("update_value_in_core", state_item.name, state_item_id, shutter_states_enum.learn_lower)
            start_learning = true
        elseif command_value == shutter_commands_enum.change_roll_direction and value >= 49 and value <= 51 then
            self:call("update_value_in_core", state_item.name, state_item_id, shutter_states_enum.change_roll_direction)
            start_learning = true
        end

        if start_learning then
            local zwave = require("zwave")
            local config_constants = require("HUB:zwave/scripts/resources/configurations/somfy_shutters")
            local parameter_numbers = config_constants.parameter_numbers
            local shutter_parameter_values = config_constants.shutter_parameter_values

            zwave.set_configuration_value(
                node_id,
                parameter_numbers.shutter_learning,
                4,
                shutter_parameter_values.start_learn
            )
        end
    end
end

self:_super_call("dimmable_light", "update_multilevel_switch", node_id, channel_id, class_id, value, target_value, target_duration)
