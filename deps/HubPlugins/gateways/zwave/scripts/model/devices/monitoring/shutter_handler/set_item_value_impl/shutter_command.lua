local self, command_item_descriptor, value = ...


-- common data
local node_id = command_item_descriptor.node_id
local channel_id = command_item_descriptor.channel_id
local class_id = command_item_descriptor.class_id

-- command item values
local command_item_name = command_item_descriptor.item_name
local command_item_id = self:call("find_item_id", command_item_name, node_id, channel_id, class_id)

-- includes
local zwave = assert(require("zwave"), "Fail to load the zwave functionality")
local shutter_commands_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/shutter_commands")
local shutter_states_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/shutter_states")
local item_names = require("HUB:zwave/scripts/resources/item_names")

-- state item values
local state_item_id = self:call("find_item_id", item_names.shutter_state, node_id, channel_id, class_id)
local state_item = self:call("find_item", item_names.shutter_state, node_id, channel_id, class_id)
assert(state_item, "No item with shutter state")


local request_value_timer = hubplugins.object("request_value_timer", "HUB:zwave/scripts/timer_handlers", node_id, channel_id, class_id)

local function start_learn_upper()
    zwave.switch_multilevel_level_change(node_id, channel_id, "up")
    request_value_timer:call("start")
end


local function start_learn_lower()
    zwave.switch_multilevel_level_change(node_id, channel_id, "down")
    request_value_timer:call("start")
end


local function start_learn_favorite()
    self:call("update_value_in_core", state_item.name, state_item_id, shutter_states_enum.learn_favorite)
end


local function start_change_roll_direction()
    zwave.set_value(node_id, class_id, channel_id, 50)
    request_value_timer:call("start")
end


local function store(current_state)
    local config_constants = require("HUB:zwave/scripts/resources/configurations/somfy_shutters")
    local parameter_numbers = config_constants.parameter_numbers
    local shutter_parameter_values = config_constants.shutter_parameter_values

    local parameter_value
    if current_state == shutter_states_enum.learn_favorite then
        parameter_value = shutter_parameter_values.store_favorite
    else
        parameter_value = shutter_parameter_values.store
    end

    zwave.set_configuration_value(
        node_id,
        parameter_numbers.shutter_learning,
        4,
        parameter_value
    )

    self:call("update_value_in_core", state_item.name, state_item_id, shutter_states_enum.idle)
    self:call("update_value_in_core", command_item_name, command_item_id, shutter_commands_enum.off)
end


local function cancel(current_state)
    local config_constants = require("HUB:zwave/scripts/resources/configurations/somfy_shutters")
    local parameter_numbers = config_constants.parameter_numbers
    local shutter_parameter_values = config_constants.shutter_parameter_values

    if current_state == shutter_states_enum.learn_upper
    or current_state == shutter_states_enum.learn_lower
    or current_state == shutter_states_enum.change_roll_direction then
        zwave.set_configuration_value(
            node_id,
            parameter_numbers.shutter_learning,
            4,
            shutter_parameter_values.cancel_limit
        )
     end

    self:call("update_value_in_core", state_item.name, state_item_id, shutter_states_enum.idle)
    self:call("update_value_in_core", command_item_name, command_item_id, shutter_commands_enum.off)
end


-- main


-- update command item value
self:call("update_value_in_core", command_item_name, command_item_id, value)

local current_state = state_item.value
local command = value

if (current_state == nil) then
   self:call("update_value_in_core", state_item.name, state_item_id, shutter_states_enum.idle)
   current_state = shutter_states_enum.idle
end

if current_state ~= shutter_states_enum.idle then
    if command == shutter_commands_enum.store then
        store(current_state)
    elseif command == shutter_commands_enum.off then
        cancel(current_state)
    else
        error("Shutter learning is busy")
    end
elseif command == shutter_commands_enum.learn_upper then
    start_learn_upper()
elseif command == shutter_commands_enum.learn_lower then
    start_learn_lower()
elseif command == shutter_commands_enum.learn_favorite then
    start_learn_favorite()
elseif command == shutter_commands_enum.change_roll_direction then
    start_change_roll_direction()
elseif command == shutter_commands_enum.off then
	cancel(current_state)
else
    error("Wrong shutter learning command")
end
