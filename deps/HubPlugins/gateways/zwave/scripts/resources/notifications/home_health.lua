local sleep_stage_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/sleep_stage")


local function sleep_stage_to_enum(event_type)
    if event_type == "sleep_stage_0_detected" then
        return sleep_stage_enum.dreaming
    elseif event_type == "sleep_stage_1_detected" then
        return sleep_stage_enum.light_sleep 
    elseif event_type == "sleep_stage_2_detected" then
        return sleep_stage_enum.medium_sleep
    elseif event_type == "sleep_stage_3_detected" then
        return sleep_stage_enum.deep_sleep 
    else
        error("Unsupported sleep stage")
    end
end

return {
    item_tag = require("HUB:zwave/scripts/resources/item_descriptor_tags").home_health,

    items_data = {
        position = {
            can_be_reset_by_idle_event = true,
            value_after_reset = "unknown",
            get_enum_values = function(event_type) 
                if event_type ~= "posture_changed" then 
                    return { event_type }
                else
                    return {}
                end 
            end,
            get_value = function (event_type, notification_value)
                if event_type == "posture_changed" then
                    return "unknown"
                else
                    return event_type
                end 
            end,
        },
        sleep_stage = {
            can_be_reset_by_idle_event = true,
            value_after_reset = sleep_stage_enum.not_sleeping,
            initial_enum = { sleep_stage_enum.not_sleeping },
            get_enum_values = function (event_type) return { sleep_stage_to_enum(event_type) } end,
            get_value = function (event_type, notification_value) return sleep_stage_to_enum(event_type) end,
        },
        voc_level_status = {
            value_after_reset = "unknown",
            should_reset_on_timeout = true,
            get_enum_values = function(event_type)
                local voc_level_status_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/voc_level_status")

                assert(event_type == "volatile_organic_compound_level", "Unsupported voc event")
                return {
                    voc_level_status_enum.clean,
                    voc_level_status_enum.slightly_poluted,
                    voc_level_status_enum.moderately_polluted,
                    voc_level_status_enum.highly_polluted,
                }
            end,
            get_value = function (event_type, notification_value) return notification_value end,
        },
        sleep_apnea = {
            can_be_reset_by_idle_event = true,
            value_after_reset = "not_detected",
            get_enum_values = function(event_type) 
                assert(event_type == "sleep_apnea_detected", "Unsupported sleep apnea event")
                local sleep_apnea_status_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/sleep_apnea_status")

                return {
                    sleep_apnea_status_enum.clean,
                    sleep_apnea_status_enum.slightly_poluted,
                    sleep_apnea_status_enum.moderately_polluted,
                }
            end,
            get_value = function (event_type, notification_value) return notification_value end,
        },
    },

    events_to_item_names = {
        leaving_bed = "position",
        sitting_on_bed = "position",
        lying_on_bed = "position",
        sitting_on_bed_edge = "position",
        posture_changed = "position",

        volatile_organic_compound_level = "voc_level_status",

        sleep_apnea_detected = "sleep_apnea",

        sleep_stage_0_detected = "sleep_stage",
        sleep_stage_1_detected = "sleep_stage",
        sleep_stage_2_detected = "sleep_stage",
        sleep_stage_3_detected = "sleep_stage",
    },
}
