local self, item_id, node_id, channel_id, class_id, value, scale, is_requested = ...

local energy_saving_mode_handler = hubplugins.object("energy_saving_mode", "HUB:zwave/scripts/helpers")
if energy_saving_mode_handler:call("is_waiting_for_high_priority_timer") then
    print("Do not react on setpoint value in energy saving mode if another action in progress")
    return
end

local is_correct_temperature = energy_saving_mode_handler:call("is_item_in_energy_saving_mode", item_id, value, scale)

if not is_correct_temperature then
    if is_requested then
        energy_saving_mode_handler:call("init_next_timer", 3000)
    else
        local item_names = require("HUB:zwave/scripts/resources/item_names")
        local energy_saving_item = self:call(
            "find_item",
            item_names.thermostat_energy_saving_mode,
            node_id, 
            channel_id, 
            class_id
        )
        
        if energy_saving_item then
            local posptponed_mode = energy_saving_mode_handler:call("utils/actual_to_postponed_mode", energy_saving_item.value)
            if posptponed_mode then
                local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
                self:call(
                    "set_item_value", 
                    energy_saving_item.id,
                    storage:call("find_item_descriptor", energy_saving_item.id),
                    posptponed_mode 
                )
            end
        end
    end
end
