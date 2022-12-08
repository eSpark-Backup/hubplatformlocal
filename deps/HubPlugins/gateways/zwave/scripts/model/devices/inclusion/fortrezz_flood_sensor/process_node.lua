local self, zwave_node = ...

local find_if = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/find_in_array_if")

-- Temperature sensor from channel 0 should be disabled
local _, channel_0 = find_if(zwave_node.channels, function(channel) return channel.id == 0 end)
if channel_0 then
    local _, cc = find_if(channel_0.classes, function(cc) return cc.id == 89 end)
    if cc then
        cc.association_groups_count = 8
        cc.association_groups = {
                {
                    commands = {
                        {
                            class_id = 128,
                            command_id = 3,
                            name = "BATTERY_REPORT"
                        },
                        {
                            class_id = 90,
                            command_id = 1,
                            name = "DEVICE_RESET_LOCALLY_NOTIFICATION"
                        },
                        {
                            class_id = 113,
                            command_id = 5,
                            name = "NOTIFICATION_REPORT"
                        },
                        {
                            class_id = 49,
                            command_id = 5,
                            name = "SENSOR_MULTILEVEL_REPORT"
                        }
                    },
                    group_id = 1,
                    group_name = "Lifeline",
                    profile = {
                        name = "General:Lifeline",
                        profile_lsb = 1,
                        profile_msb = 0
                    }
                },
                {
                    commands = {
                        {
                            class_id = 32,
                            command_id = 1,
                            name = "BASIC_SET"
                        }
                    },
                    group_id = 2,
                    group_name = "Water Leak",
                    profile = {
                        name = "Control:Key01",
                        profile_lsb = 1,
                        profile_msb = 32
                    }
                },
                {
                    commands = {
                        {
                            class_id = 32,
                            command_id = 1,
                            name = "BASIC_SET"
                        }
                    },
                    group_id = 3,
                    group_name = "Over Temperature",
                    profile = {
                        name = "Control:Key02",
                        profile_lsb = 2,
                        profile_msb = 32
                    }
                },
                {
                    commands = {
                        {
                            class_id = 32,
                            command_id = 1,
                            name = "BASIC_SET"
                        }
                    },
                    group_id = 4,
                    group_name = "Under Temperature",
                    profile = {
                        name = "Control:Key03",
                        profile_lsb = 3,
                        profile_msb = 32
                    }
                },
                {
                    commands = {
                        {
                            class_id = 49,
                            command_id = 5,
                            name = "SENSOR_MULTILEVEL_REPORT"
                        }
                    },
                    group_id = 5,
                    group_name = "Temp-Upon Change",
                    profile = {
                        name = "Sensor:Air Temperature",
                        profile_lsb = 1,
                        profile_msb = 49
                    }
                },
                {
                    commands = {
                        {
                            class_id = 49,
                            command_id = 5,
                            name = "SENSOR_MULTILEVEL_REPORT"
                        }
                    },
                    group_id = 6,
                    group_name = "Temp-Every 5 min",
                    profile = {
                        name = "Sensor:Air Temperature",
                        profile_lsb = 1,
                        profile_msb = 49
                    }
                },
                {
                    commands = {
                        {
                            class_id = 49,
                            command_id = 5,
                            name = "SENSOR_MULTILEVEL_REPORT"
                        }
                    },
                    group_id = 7,
                    group_name = "Temp-Every 20 min",
                    profile = {
                        name = "Sensor:Air Temperature",
                        profile_lsb = 1,
                        profile_msb = 49
                    }
                },
                {
                    commands = {
                        {
                            class_id = 49,
                            command_id = 5,
                            name = "SENSOR_MULTILEVEL_REPORT"
                        }
                    },
                    group_id = 8,
                    group_name = "Temp-Every 60 min",
                    profile = {
                        name = "Sensor:Air Temperature",
                        profile_lsb = 1,
                        profile_msb = 49
                    }
                }
            }
    end
end

return self:_super_call("default", "process_node", zwave_node)
