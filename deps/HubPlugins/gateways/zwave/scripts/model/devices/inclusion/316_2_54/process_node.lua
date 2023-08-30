local self, zwave_node = ...

local find_if = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/find_in_array_if")

-- Temperature sensor from channel 0 should be disabled
local _, channel_0 = find_if(zwave_node.channels, function(channel) return channel.id == 0 end)
if channel_0 then
    local _, cc = find_if(channel_0.classes, function(cc) return cc.id == 89 end)
    if cc then
        cc.association_groups_count = 1
        cc.association_groups = {
                {
                    commands = {
                        {
                            class_id = 128,
                            command_id = 3,
                            name = "BATTERY_REPORT"
                        },
                        {
                            class_id = 113,
                            command_id = 5,
                            name = "NOTIFICATION_REPORT"
                        },
                        {
                            class_id = 90,
                            command_id = 1,
                            name = "DEVICE_RESET_LOCALLY_NOTIFICATION"
                        }
                    },
                    group_id = 1,
                    group_name = "Lifeline",
                    profile = {
                        name = "General:Lifeline",
                        profile_lsb = 1,
                        profile_msb = 0
                    }
                }
            }
    end
end

return self:_super_call("default", "process_node", zwave_node)
