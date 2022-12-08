local base = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/base_fixture")()

local item_names = require("HUB:zwave/scripts/resources/item_names")
local class_ids = require("HUB:zwave/scripts/resources/class_ids")

local fixture = setmetatable({}, {  __index = base })

function fixture:create_base_node()
    return {
        id = "9000",
        channels = {
            {
                id = 0,
                classes = {
                    { id = 94, },
                    { id = class_ids.switch_binary, },
                    { id = 133, },
                    { id = 142, },
                    { id = 89, },
                    { id = 134, },
                    { id = 114, },
                    { id = 96 },
                    -- { id = class_ids.configuration },
                    {
                        id = class_ids.central_scene,
                        buttons = {
                            {
                                button_id = 1,
                                button_attributes = {
                                    "press_1_time",
                                    "released",
                                    "held_down",
                                    "press_2_times",
                                    "press_3_times" 
                                }
                            },
                            {
                                button_id = 2,
                                button_attributes = {
                                    "press_1_time",
                                    "released",
                                    "held_down",
                                    "press_2_times",
                                    "press_3_times" 
                                }
                            }
                        }
                    },
                    { 
                        id = class_ids.multilevel_sensor,
                        sensors = {
                            {
                                sensor_type = "air_temperature",
                                scales = { "celsius" }
                            },
                            {
                                sensor_type = "voltage",
                                scales = { "volt" }
                            }
                        }
                    },
                    {
                        id = class_ids.notification,
                        notifications =  {
                            {
                                event_types = { "intrusion" },
                                type = "home_security",
                                unknown_event_types =  {}
                            }
                        },
                    }
                },
            },
            {
                id = 1,
                classes = {
                    { id = 94 },
                    { id = 133 },
                    { id = 142 },
                    { id = 89 },
                    {
                        id = class_ids.notification,
                        notifications =  {
                            {
                                event_types = { "intrusion" },
                                type = "home_security",
                                unknown_event_types =  {}
                            }
                        },
                    }
                }
            },
            {
                id = 2,
                classes = { 
                    { id = 94 },
                    { id = 133 },
                    { id = 142 },
                    { id = 89 },
                    {
                        id = class_ids.notification,
                        notifications =  {
                            {
                                event_types = {
                                    "intrusion",
                                },
                                type = "home_security",
                                unknown_event_types =  {}
                            }
                        },
                    }
                }
            },
            {
                id = 3,
                classes = {
                    { id = 94 },
                    { id = 133 },
                    { id = 142 },
                    { id = 89 },
                    { 
                        id = class_ids.multilevel_sensor,
                        sensors = {}
                    }
                }
            },
            {
                id = 4,
                classes = { 
                    { id = 94 },
                    { id = 133 },
                    { id = 142 },
                    { id = 89 },
                    { 
                        id = class_ids.multilevel_sensor,
                        sensors = {}
                    }
                }
            },
            {
                id = 5,
                classes = { 
                    { id = 94 },
                    { id = class_ids.switch_binary },
                    { id = 133 },
                    { id = 142 },
                    { id = 89 }
                }
            },
            {
                id = 6,
                classes = { 
                    { id = 94 },
                    { id = class_ids.switch_binary },
                    { id = 133 },
                    { id = 142 },
                    { id = 89 }
                }
            },
            {
                id = 7,
                classes = { 
                    { id = 94 },
                    { id = 133 },
                    { id = 142 },
                    { id = 89 },
                    {
                        id = class_ids.multilevel_sensor,
                        sensors = {}
                    },
                    {
                        id = class_ids.notification,
                        notifications = {
                            {
                                type = "system",
                                unknown_event_types = {},
                                event_types = { "system_software_failure" }
                            }
                        }
                    }
                }
            }
        },
        generic_type = "GENERIC_TYPE_SENSOR_NOTIFICATION",
        specific_type = "SPECIFIC_TYPE_NOTIFICATION_SENSOR",
        is_battery_powered = false,
        manufacturer_id = 271,
        product_type = 1282,
        product_id = 8192,
        security =  { "s0" },
        security_class_started =  { "s0" },
    }
end

local function append_extra_channels(node, extra_channels_count)
    for i = 1, extra_channels_count do
        table.insert(node.channels, {
            id = 7 + i,
            classes = { 
                { id = 94 },
                { id = 133 },
                { id = 142 },
                { id = 89 },
                {
                    id = class_ids.multilevel_sensor,
                    sensors = {}
                },
                {
                    id = class_ids.notification,
                    notifications = {
                        {
                            type = "system",
                            unknown_event_types = {},
                            event_types = { "system_software_failure" }
                        }
                    }
                }
            }
        })
    end
end

function fixture:create_dht22_node()
    local node = self:create_base_node()

    for _, class in ipairs(node.channels[1].classes) do
        if class.id == class_ids.multilevel_sensor then
            table.insert(class.sensors, { 
                    sensor_type = "humidity",
                    scales = { "percent" }
                })
            break
        end
    end

    append_extra_channels(node, 2)

    return node
end

function fixture:create_ds18s20_node(ds18s20_sensors_count)
    local node = self:create_base_node()

    append_extra_channels(node, ds18s20_sensors_count)

    return node
end

function fixture:create_base_expected_devices()
    return {
        {
            name = "Fibaro FGBS Smart Implant",
            category = "temperature",
            items = {
                {
                    name = item_names.temp,
                    cc_id = class_ids.multilevel_sensor,
                    channel_id = 0
                },
                {
                    name = item_names.temp,
                    cc_id = class_ids.multilevel_sensor,
                    channel_id = 7,
                    is_merged = true
                },
                {
                    name = "sw_state",
                    cc_id = class_ids.notification,
                    tag = "system",
                    channel_id = 7
                },
            },
            node_id = "9000",
            battery_powered = false,
            security = "middle",

            subdevices = {
                {
                    name = "Switch 1",
                    category = "switch",
                    subcategory = "relay",
                    items = {
                        {
                            name = item_names.switch,
                            cc_id = class_ids.switch_binary,
                            channel_id = 5
                        },
                        {
                            name = item_names.switch,
                            cc_id = class_ids.switch_binary,
                            channel_id = 0,
                            is_merged = true
                        }
                    },
                },
                {
                    name = "Switch 2",
                    category = "switch",
                    subcategory = "relay",
                    items = {
                        {
                            name = item_names.switch,
                            cc_id = class_ids.switch_binary,
                            channel_id = 6
                        }
                    },
                },
                {
                    name = "Voltage Level Sensor 1",
                    category = "voltage_sensor",
                    items = {
                        {
                            name = item_names.electric_meter_volt,
                            cc_id = class_ids.multilevel_sensor,
                            channel_id = 3
                        },
                        {
                            name = item_names.electric_meter_volt,
                            cc_id = class_ids.multilevel_sensor,
                            channel_id = 0,
                            is_merged = true
                        }
                    }
                },
                {
                    name = "Voltage Level Sensor 2",
                    category = "voltage_sensor",
                    items = {
                        {
                            name = item_names.electric_meter_volt,
                            cc_id = class_ids.multilevel_sensor,
                            channel_id = 4
                        }
                    }
                },
                {
                    name = "Generic Sensor 1",
                    category = "generic_sensor",
                    items = {
                        {
                            name = item_names.intrusion_alarm,
                            cc_id = class_ids.notification,
                            tag = "home_security",
                            channel_id = 1
                        },
                        {
                            name = item_names.intrusion_alarm,
                            cc_id = class_ids.notification,
                            tag = "home_security",
                            channel_id = 0,
                            is_merged = true
                        },
                        {
                            name = item_names.security_threat,
                            tag = "home_security.security_tag",
                            cc_id = class_ids.notification,
                            channel_id = 1,
                            main_item_name = item_names.intrusion_alarm
                        }
                    }
                },
                {
                    name = "Generic Sensor 2",
                    category = "generic_sensor",
                    items = {
                        {
                            name = item_names.intrusion_alarm,
                            cc_id = class_ids.notification,
                            tag = "home_security",
                            channel_id = 2
                        },
                        {
                            name = item_names.security_threat,
                            tag = "home_security.security_tag",
                            cc_id = class_ids.notification,
                            channel_id = 2,
                            main_item_name = item_names.intrusion_alarm
                        }
                    }
                },
                {
                    name = "Scene Controller",
                    category = "scene_controller",
                    items = {
                        {
                            name = item_names.button_state,
                            cc_id = class_ids.central_scene,
                            channel_id = 0,
                            tag = "1"
                        },
                        {
                            name = item_names.button_state,
                            cc_id = class_ids.central_scene,
                            channel_id = 0,
                            tag = "2"
                        }
                    }
                }
            }
        }
    }
end

return fixture