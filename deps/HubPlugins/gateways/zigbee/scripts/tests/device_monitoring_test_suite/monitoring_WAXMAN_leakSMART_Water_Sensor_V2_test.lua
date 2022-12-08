local core = require("core")
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

local water_leak_alarm_enum = loadfile("HUB:zigbee/scripts/resources/enums/water_leak_alarm_event")()

local storage_helper = hubplugins.object("storage_helper", "HUB:zigbee/scripts/utils")

local water_leak_alarm_item_id =
    storage_helper:call(
    "find_item_id",
    {
        item_name = "water_leak_alarm",
        node_id = 5,
        endpoint_id = 1,
        cluster_name = "appliance_events_and_alerts",
        attribute_name = ""
    }
)

tests.assert_ne(water_leak_alarm_item_id, nil, "Try to find item id")

local security_threat_item_id =
    storage_helper:call(
    "find_item_id",
    {
        item_name = "water_leak_alarm",
        node_id = 5,
        endpoint_id = 1,
        cluster_name = "appliance_events_and_alerts",
        attribute_name = "",
        tag = ".security_tag"
    }
)

tests.assert_ne(security_threat_item_id, nil, "Try to find item id")

core.update_item_value = tests.create_mock()

--check water leak detected
events_handler(
    {
        alert_id = 129,
        category = "warning",
        cluster_name = "appliance_events_and_alerts",
        command_id = 1,
        command_name = "AlertsNotification",
        endpoint_id = 1,
        event = "alert_received",
        manufacturer_specific_bits = 0,
        node_id = 5,
        presence_recovery = "presence"
    }
)

tests.assert_called(
    core.update_item_value,
    "check if water_leak_alarm value changed",
    {
        { water_leak_alarm_item_id, water_leak_alarm_enum.water_leak_detected },
        { security_threat_item_id, true }
    }
)

--check no water leak
events_handler(
    {
        alert_id = 129,
        category = "warning",
        cluster_name = "appliance_events_and_alerts",
        command_id = 1,
        command_name = "AlertsNotification",
        endpoint_id = 1,
        event = "alert_received",
        manufacturer_specific_bits = 0,
        node_id = 5,
        presence_recovery = "recovery"
    }
)

tests.assert_called(
    core.update_item_value,
    "check if water_leak_alarm value changed",
    {
        { water_leak_alarm_item_id, water_leak_alarm_enum.no_water_leak },
        { security_threat_item_id, false }
    }
)

--check invalid alert_id
events_handler(
    {
        alert_id = 130,
        category = "warning",
        cluster_name = "appliance_events_and_alerts",
        command_id = 1,
        command_name = "AlertsNotification",
        endpoint_id = 1,
        event = "alert_received",
        manufacturer_specific_bits = 0,
        node_id = 5,
        presence_recovery = "recovery"
    }
)

tests.assert_called(
    core.update_item_value,
    "check if water_leak_alarm value changed",
    {}
)

--check empty alerts list event
events_handler(
    {
        cluster_name = "appliance_events_and_alerts",
        command_id = 0,
        command_name = "GetAlertsResponse",
        endpoint_id = 1,
        event = "empty_alerts_list_received",
        node_id = 5,
    }
)

tests.assert_called(
    core.update_item_value,
    "check if water_leak_alarm value changed",
    {
        { water_leak_alarm_item_id, water_leak_alarm_enum.no_water_leak },
        { security_threat_item_id, false }
    }
)
