local base_token_item_handler_mt = require("HUB:zwave/scripts/resources/notifications/item_handlers/base_token_item_handler_mt")

local function program_status_to_enum(event_type)
    local program_status_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/program_status")
            
    if event_type == "program_started" then
        return { program_status_enum.started }
    elseif event_type == "program_in_progress" then
        return { program_status_enum.in_progress } 
    elseif event_type == "program_completed" then
        return { program_status_enum.completed } 
    else
        error("Unsupported program status")
    end
end

local function event_to_program_name(event_type)
    local program_name_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/program_name")
    if event_type == "failure_to_set_target_temperature" then
        return program_name_enum.target_temperature_setting
    elseif event_type == "water_supply_failure" then
        return program_name_enum.water_supply
    elseif event_type == "boiling_failure" then
        return program_name_enum.boiling
    elseif event_type == "washing_failure" then
        return program_name_enum.washing
    elseif event_type == "rinsing_failure" then
        return program_name_enum.rinsing
    elseif event_type == "draining_failure" then
        return program_name_enum.draining
    elseif event_type == "spinning_failure" then
        return program_name_enum.spinning
    elseif event_type == "drying_failure" then
        return program_name_enum.drying
    elseif event_type == "fan_failure" then
        return program_name_enum.fan
    elseif event_type == "compressor_failure" then
        return program_name_enum.compressor
    else
        error("Unsupported program name")
    end
end

return {
    item_tag = require("HUB:zwave/scripts/resources/item_descriptor_tags").appliance,

    items_data = {
        program_status = setmetatable( {
                can_be_reset_by_idle_event = true, 
                value_after_reset = "idle",
                get_enum_values = program_status_to_enum,
                get_value = function (event_type, notification_value) return program_status_to_enum(event_type)[1] end,
            }, 
            base_token_item_handler_mt ),
        appliance_status = setmetatable( {
                can_be_reset_by_idle_event = true, 
                value_after_reset = "idle",
            }, 
            base_token_item_handler_mt ),
        program_failures = {
            can_be_reset_by_idle_event = true,
            dictionary_value_after_reset = function(old_value, opt_event_type)
                local program_failed_status_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/program_failed_status")
                local result = {}
                for key, value in pairs(old_value) do
                    if not opt_event_type or key == event_to_program_name(opt_event_type) then
                        result[key] = program_failed_status_enum.ok
                    else
                        result[key] = value
                    end
                end
                
                return result
            end,
            get_initial_dictionary_value = function(event_type) 
                local program_failed_status_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/program_failed_status")
                return { [event_to_program_name(event_type)] = program_failed_status_enum.ok }
            end,
            get_value = function (event_type, notification_value)
                local program_failed_status_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/program_failed_status")
                return { [event_to_program_name(event_type)] = program_failed_status_enum.failed }
            end,
        },
        maintenance_state = hubplugins.weak_require("HUB:zwave/scripts/resources/notifications/item_handlers/maintenance_state"),
    },

    events_to_item_names = {
        program_started = "program_status",
        program_in_progress = "program_status",
        program_completed = "program_status",

        replace_main_filter = "maintenance_state",

        supplying_water = "appliance_status",
        boiling = "appliance_status",
        washing = "appliance_status",
        rinsing = "appliance_status",
        draining = "appliance_status",
        spinning = "appliance_status",
        drying = "appliance_status",

        failure_to_set_target_temperature = "program_failures",
        water_supply_failure = "program_failures",
        boiling_failure = "program_failures",
        washing_failure = "program_failures",
        rinsing_failure = "program_failures",
        draining_failure = "program_failures",
        spinning_failure = "program_failures",
        drying_failure = "program_failures",
        fan_failure = "program_failures",
        compressor_failure = "program_failures"
    },
}
