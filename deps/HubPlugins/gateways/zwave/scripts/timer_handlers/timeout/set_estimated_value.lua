local params = ...

local storage = hubplugins.object( "storage_helper", "HUB:zwave/scripts/utils" )
local descriptor = storage:call("find_item_descriptor", params.item_id)

assert(descriptor)

local in_progress = params.spent_time < params.target_duration
local estimated_value = in_progress and
             params.value + ( params.target_value - params.value ) * params.spent_time / params.target_duration or
             params.target_value

print("set_estimated_value: node_id " .. descriptor.node_id .. 
    ", channel_id " .. descriptor.channel_id .. 
    ", class_id " .. descriptor.class_id ..
    ", estimated_value " .. estimated_value ..
    ", target_value " .. params.target_value ..
    ", spent_time " ..  params.spent_time ..
    ", target_duration " .. params.target_duration)

if in_progress then
    local estimated_value_timer = hubplugins.object("estimated_value_timer", "HUB:zwave/scripts/timer_handlers", params.item_id)
    estimated_value_timer:call("continue", params)
end

local handler = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_handler_monitoring")(descriptor.node_id)
handler:call("update_value_in_core", descriptor.item_name, params.item_id, estimated_value )
