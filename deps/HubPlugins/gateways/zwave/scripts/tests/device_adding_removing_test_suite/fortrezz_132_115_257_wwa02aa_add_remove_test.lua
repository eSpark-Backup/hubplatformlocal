local item_names = require("HUB:zwave/scripts/resources/item_names")
local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local item_descriptor_tags = require("HUB:zwave/scripts/resources/item_descriptor_tags")
local events_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/events/events_handling")
local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/base_fixture")()
local zwave = require("zwave")
local tests = require( "tests" )

zwave.request_node_values = tests.create_mock()

local node = {
    id = "9000",
    channels =  {
        {
            id = 0,
            classes =  {
                { id = class_ids.basic, },
                { id = class_ids.battery, },
                {
                    id = class_ids.notification,
                    notifications =  {
                        {
                            event_types = {
                                "state_idle",
                                "water_leak_detected"
                            },
                            type = "water_alarm",
                            unknown_event_types =  {}
                        },
                        {
                            event_types = {
                                "overheat_detected"
                            },
                            type = "heat_alarm",
                            unknown_event_types =  {}
                        }
                    },
                },
                {
                    id = class_ids.multilevel_sensor,
                    sensors =  {
                        {
                            scales =  { "celsius" },
                            sensor_type = "air_temperature"
                        }
                    },
                },
            },
        }
    },
    security_class_started = { "none" },
    security = { "none" },


    generic_type = "GENERIC_TYPE_SENSOR_NOTIFICATION",
    specific_type = "SPECIFIC_TYPE_NOTIFICATION_SENSOR",
    is_battery_powered = true,
    manufacturer_id = 132,
    product_type = 115,
    product_id = 257,
}

fixture:add_node(node)

local expected_devices = {
    {
        name = "Flood and Temp Sensor",
        category = "security_sensor",
        subcategory = "leak",
        items = {
            {
                name = item_names.battery,
                cc_id = class_ids.battery,
                channel_id = 0
            },
            {
                name = item_names.basic,
                cc_id = class_ids.basic,
                channel_id = 0
            },
            {
                name = item_names.battery_maintenance_state,
                tag = "power_management",
                cc_id = class_ids.notification,
                channel_id = 0
            },
            {
                name = item_names.power_state,
                tag = "power_management",
                cc_id = class_ids.notification,
                channel_id = 0
            },
            {
                name = "hw_state",
                cc_id = class_ids.notification,
                tag = "system",
                channel_id = 0
            },
            {
                name = item_names.water_leak_alarm,
                tag = item_descriptor_tags.water_alarm,
                cc_id = class_ids.notification,
                channel_id = 0,
            },
            {
                name = item_names.security_threat,
                tag = "water_alarm.security_tag",
                cc_id = class_ids.notification,
                channel_id = 0,
                main_item_name = item_names.water_leak_alarm
            }
        },
        node_id = "9000",
        battery_powered = true,
        security = "low",

        subdevices = {
            {
                name = "Temperature Sensor",
                category = "temperature",
                subcategory = "",
                items = {
                    {
                        name = item_names.temp,
                        cc_id = class_ids.multilevel_sensor,
                        channel_id = 0
                    }
                },
            },
            {
                name = "Heat Sensor",
                category = "security_sensor",
                subcategory = "heat",
                items = {
                    {
                        name = item_names.heat_alarm,
                        tag = item_descriptor_tags.heat_alarm,
                        cc_id = class_ids.notification,
                        channel_id = 0,
                    },
                    {
                        name = item_names.security_threat,
                        tag = "heat_alarm.security_tag",
                        cc_id = class_ids.notification,
                        channel_id = 0,
                        main_item_name = item_names.heat_alarm
                    }
                },
            },
        }
    }
}

local check_context = {
    descriptors = {},
    item_ids = {},
    device_ids = {},
    tests = tests
}

local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"
loadfile(test_suite_dir .. "check_zwave_node_adding_result")(expected_devices, check_context)

fixture:remove_node(node)

loadfile(test_suite_dir .. "check_zwave_node_removing_result")(check_context)
