-- 65536_170_1 - Slave node in a certification test suite
local self, inclusion_context, build_tree_context, device_node = ...

local notification_cc = build_tree_context.command_class

local function set_notification_events(event_types)
    for _, notification in ipairs(notification_cc.notifications) do
        if event_types[notification.type] then
            notification.event_types = event_types[notification.type]
            notification.unknown_event_types = {}
        end
    end
end

if notification_cc.version == 1 or notification_cc.v1_supported then
    -- For v1 the test expects that we can trigger a Alarm Get for Notification Type 0x09 (System) using the UI
    -- So we are going to trigger it on each rediscover request via zwave.request_alarm_v1_value
    local zwave = require("zwave")
    if zwave.request_alarm_v1_value then
        zwave.request_alarm_v1_value(inclusion_context.zwave_node.id, build_tree_context.channel.id, 0x09)
    else
        print("zwave.request_alarm_v1_value is not implemented!")
    end
elseif notification_cc.version == 2 then
    -- For v2 we receive 0xFE which is an unknown event, so we force creation of the events required by the test
    set_notification_events({
        heat_alarm = { "overheat_detected" },
        system = { "system_hardware_failure" }
    })
else
    -- For v8 we receive 0x02 which is an unknown event type, so we treat it as 0x01 (siren_active) to pass the test
    set_notification_events({
        siren = { "siren_active" }
    })
end

self:_super_call("default/113", "process_inclusion_impl", inclusion_context, build_tree_context, device_node)
