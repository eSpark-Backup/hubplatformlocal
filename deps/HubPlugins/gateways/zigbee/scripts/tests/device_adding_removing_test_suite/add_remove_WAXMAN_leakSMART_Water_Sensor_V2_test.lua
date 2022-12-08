local tests = require("tests")

local cluster_appliance_events_and_alerts = {
    name = "appliance_events_and_alerts",
    attributes = {},
    commands = {
        {
            _id = 0,
            cluster_id = 2818,
            is_mfg_specific = false,
            mfg_code = 65535,
            name = "Unknown",
            type = "generated"
        },
        {
            _id = 0,
            cluster_id = 2818,
            is_mfg_specific = false,
            mfg_code = 65535,
            name = "Unknown",
            type = "received"
        },
        {
            _id = 1,
            cluster_id = 2818,
            is_mfg_specific = false,
            mfg_code = 65535,
            name = "Unknown",
            type = "generated"
        },
        {
            _id = 2,
            cluster_id = 2818,
            is_mfg_specific = false,
            mfg_code = 65535,
            name = "Unknown",
            type = "generated"
        }
    }
}

local cluster_temperature_measurement = {
    name = "temperature_measurement",
    attributes = {
        {
            _id = 0,
            cluster_id = 1026,
            formatted_value = 2409,
            is_mfg_specific = false,
            mfg_code = 65535,
            name = "MeasuredValue",
            type = "int16_t",
            value = "2409"
        },
        {
            _id = 1,
            cluster_id = 1026,
            formatted_value = -4000,
            is_mfg_specific = false,
            mfg_code = 65535,
            name = "MinMeasuredValue",
            type = "int16_t",
            value = "-4000"
        },
        {
            _id = 2,
            cluster_id = 1026,
            formatted_value = 12500,
            is_mfg_specific = false,
            mfg_code = 65535,
            name = "MaxMeasuredValue",
            type = "int16_t",
            value = "12500"
        }
    },
    commands = {}
}

local cluster_power_configuration = {
    name = "power_configuration",
    attributes = {
        {
            _id = 32,
            cluster_id = 1,
            formatted_value = 43,
            is_mfg_specific = false,
            mfg_code = 65535,
            name = "BatteryVoltage",
            type = "uint8_t",
            value = "43"
        },
        {
            _id = 33,
            cluster_id = 1,
            formatted_value = 180,
            is_mfg_specific = false,
            mfg_code = 65535,
            name = "BatteryPercentageRemaining",
            type = "uint8_t",
            value = "180"
        },
        {
            _id = 53,
            cluster_id = 1,
            formatted_value = "1",
            is_mfg_specific = false,
            mfg_code = 65535,
            name = "BatteryAlarmMask",
            type = "bitmap8_t",
            value = "1"
        },
        {
            _id = 54,
            cluster_id = 1,
            formatted_value = "32",
            is_mfg_specific = false,
            mfg_code = 65535,
            name = "BatteryVoltageMinThreshold",
            type = "uint8_t",
            value = "32"
        },
        {
            _id = 62,
            cluster_id = 1,
            formatted_value = "unknown",
            is_mfg_specific = false,
            mfg_code = 65535,
            name = "BatteryAlarmState",
            typr = "unknown",
            value = "unknown"
        }
    },
    commands = {}
}

local endpoint = {
    id = 1,
    clusters = {
        cluster_appliance_events_and_alerts,
        cluster_temperature_measurement,
        cluster_power_configuration
    }
}

local node = {
    id = 5,
    manufacturer_id = "WAXMAN",
    model_id = "leakSMART Water Sensor V2",
    security = false,
    is_reachable = false,
    is_battery_powered = true,
    endpoints = {endpoint}
}

local events_handler = assert(loadfile("HUB:zigbee/scripts/events/zigbee/events_handling"))

local zigbee = require("zigbee")

-- prepare zigbee
zigbee.add_node(node)

-- add node
events_handler(
    {
        event = "node_added",
        node_id = 5
    }
)

-- check adding result

local water_leak_alarm_enum = loadfile("HUB:zigbee/scripts/resources/enums/water_leak_alarm_event")()

local expected_items_water_leak_sensor = {
    {
        name = "battery",
        endpoint_id = 1,
        cluster_name = "power_configuration",
        attribute_name = "BatteryPercentageRemaining",
        value_type = "int",
        value = 100
    },
    {
        name = "water_leak_alarm",
        endpoint_id = 1,
        cluster_name = "appliance_events_and_alerts",
        attribute_name = "",
        value_type = "token",
        enum = {
            water_leak_alarm_enum.unknown,
            water_leak_alarm_enum.no_water_leak,
            water_leak_alarm_enum.water_leak_detected
        },
        value = water_leak_alarm_enum.unknown
    },
    {
        name = "security_threat",
        endpoint_id = 1,
        cluster_name = "appliance_events_and_alerts",
        attribute_name = "",
        tag = ".security_tag",
        main_item_name = "water_leak_alarm"
    }
}

local expected_items_temperature_sensor = {
    {
        name = "temp",
        endpoint_id = 1,
        cluster_name = "temperature_measurement",
        attribute_name = "MeasuredValue",
        value_type = "temperature",
        value = {value = 0, scale = "celsius"}
    }
}

local expected_settings = {
    {
        name = "temperature_calibration",
        endpoint_id = 1,
        cluster_name = "temperature_measurement",
        attribute_name = "MeasuredValue",
        label = {
            lang_tag = "zigbee_device_setting_temperature_calibration_label",
            text = "Zigbee device temperature calibration"
        },
        description = {
            lang_tag = "zigbee_device_setting_temperature_calibration_description",
            text = "Value to calibrate sensor temperature"
        },
        value_type = "temperature",
        value = {
            value = 0,
            scale = "celsius"
        },
        value_default = {
            value = 0,
            scale = "celsius"
        },
        value_min = {
            value = -2.5,
            scale = "celsius"
        },
        value_max = {
            value = 2.5,
            scale = "celsius"
        }
    }
}

local expected_devices = {
    {
        name = "Water Leak Sensor",
        category = "security_sensor",
        subcategory = "leak",
        items = expected_items_water_leak_sensor,
        node_id = 5,
        battery_powered = true,
        subdevices = {
            {
                name = "Temperature Sensor",
                category = "temperature",
                subcategory = "",
                settings = expected_settings,
                items = expected_items_temperature_sensor,
                node_id = 5,
                battery_powered = true
            }
        }
    }
}

local check_context = {
    descriptors = {},
    item_ids = {},
    device_ids = {},
    setting_ids = {}
}

local test_suite_dir = "HUB:zigbee/scripts/tests/device_adding_removing_test_suite/"

loadfile(test_suite_dir .. "check_zigbee_node_adding_result")(expected_devices, check_context)

-- remove node
events_handler(
    {
        event = "node_removed",
        node_id = 5,
        manufacturer_id = "WAXMAN",
        model_id = "leakSMART Water Sensor V2"
    }
)

-- check node removing result
loadfile(test_suite_dir .. "check_zigbee_node_removing_result")(check_context)
