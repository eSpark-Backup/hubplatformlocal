local tests = require( "tests" )

local cluster_door_lock = {
    name = "door_lock",
    attributes = {
        {
            cluster_id = 257,
            formatted_value = "locked",
            id = 0,
            is_mfg_sp2ecific = false,
            name = "LockState",
            type = "enum8_t",
            value = "1"
        },
        {
            cluster_id = 257,
            formatted_value = "other",
            id = 1,
            is_mfg_sp2ecific = false,
            name = "LockType",
            type = "enum8_t",
            value = "2"
        },
        {
            cluster_id = 257,
            formatted_value = "enabled",
            id = 2,
            is_mfg_sp2ecific = false,
            name = "ActuatorEnabled",
            type = "bool",
            value = "true"
        },
        {
            cluster_id = 257,
            formatted_value = "10",
            id = 16,
            is_mfg_sp2ecific = false,
            name = "NumberOfLogRecordsSupported",
            type = "uint16_t",
            value = "10"
        },
        {
            cluster_id = 257,
            formatted_value = 33,
            id = 17,
            is_mfg_sp2ecific = false,
            name = "NumberOfTotalUsersSupported",
            type = "uint16_t",
            value = "33"
        },
        {
            cluster_id = 257,
            formatted_value = 33,
            id = 18,
            is_mfg_sp2ecific = false,
            name = "NumberOfPINUsersSupported",
            type = "uint16_t",
            value = "33"
        },
        {
            cluster_id = 257,
            formatted_value = "7",
            id = 20,
            is_mfg_sp2ecific = false,
            name = "NumberOfWeekDaySchedulesSupportedPerUser",
            type = "uint8_t",
            value = "7"
        },
        {
            cluster_id = 257,
            formatted_value = "1",
            id = 21,
            is_mfg_sp2ecific = false,
            name = "NumberOfYearDaySchedulesSupportedPerUser",
            type = "uint8_t",
            value = "1"
        },
        {
            cluster_id = 257,
            formatted_value = 8,
            id = 23,
            is_mfg_sp2ecific = false,
            name = "MaxPINCodeLength",
            type = "uint8_t",
            value = "8"
        },
        {
            cluster_id = 257,
            formatted_value = 4,
            id = 24,
            is_mfg_sp2ecific = false,
            name = "MinPINCodeLength",
            type = "uint8_t",
            value = "4"
        },
        {
            cluster_id = 257,
            formatted_value = "true",
            id = 32,
            is_mfg_sp2ecific = false,
            name = "EnableLogging",
            type = "bool",
            value = "true"
        },
        {
            cluster_id = 257,
            formatted_value = "2",
            id = 34,
            is_mfg_sp2ecific = false,
            name = "LEDSettings",
            type = "uint8_t",
            value = "2"
        },
        {
            cluster_id = 257,
            formatted_value = "0",
            id = 35,
            is_mfg_sp2ecific = false,
            name = "AutoRelockTime",
            type = "uint32_t",
            value = "0"
        },
        {
            cluster_id = 257,
            formatted_value = "0",
            id = 36,
            is_mfg_sp2ecific = false,
            name = "SoundVolume",
            type = "uint8_t",
            value = "0"
        },
        {
            cluster_id = 257,
            formatted_value = "0",
            id = 37,
            is_mfg_sp2ecific = false,
            name = "OperatingMode",
            type = "enum8_t",
            value = "0"
        },
        {
            cluster_id = 257,
            formatted_value = "1",
            id = 38,
            is_mfg_sp2ecific = false,
            name = "SupportedOperatingModes",
            type = "bitmap16_t",
            value = "1"
        },
        {
            cluster_id = 257,
            formatted_value = "129",
            id = 39,
            is_mfg_sp2ecific = false,
            name = "DefaultConfigurationRegister",
            type = "bitmap16_t",
            value = "129"
        },
        {
            cluster_id = 257,
            formatted_value = "true",
            id = 40,
            is_mfg_sp2ecific = false,
            name = "EnableLocalProgramming",
            type = "bool",
            value = "true"
        },
        {
            cluster_id = 257,
            formatted_value = "3",
            id = 48,
            is_mfg_sp2ecific = false,
            name = "WrongCodeEntryLimit",
            type = "uint8_t",
            value = "3"
        },
        {
            cluster_id = 257,
            formatted_value = "60",
            id = 49,
            is_mfg_sp2ecific = false,
            name = "UserCodeTemporaryDisableTime",
            type = "uint8_t",
            value = "60"
        },
        {
            cluster_id = 257,
            formatted_value = "true",
            id = 50,
            is_mfg_sp2ecific = false,
            name = "SendPINOverTheAir",
            type = "bool",
            value = "true"
        },
        {
            cluster_id = 257,
            formatted_value = "false",
            id = 51,
            is_mfg_sp2ecific = false,
            name = "RequirePINforRFOperation",
            type = "bool",
            value = "false"
        },
        {
            cluster_id = 257,
            formatted_value = "0",
            id = 52,
            is_mfg_sp2ecific = false,
            name = "SecurityLevel",
            type = "enum8_t",
            value = "0"
        },
        {
            cluster_id = 257,
            formatted_value = "127",
            id = 64,
            is_mfg_sp2ecific = false,
            name = "AlarmMask",
            type = "bitmap16_t",
            value = "127"
        },
        {
            cluster_id = 257,
            formatted_value = "65535",
            id = 65,
            is_mfg_sp2ecific = false,
            name = "KeypadOperationEventMask",
            type = "bitmap16_t",
            value = "65535"
        },
        {
            cluster_id = 257,
            formatted_value = "65535",
            id = 66,
            is_mfg_sp2ecific = false,
            name = "RFOperationEventMask",
            type = "bitmap16_t",
            value = "65535"
        },
        {
            cluster_id = 257,
            formatted_value = "65535",
            id = 67,
            is_mfg_sp2ecific = false,
            name = "ManualOperationEventMask",
            type = "bitmap16_t",
            value = "65535"
        },
        {
            cluster_id = 257,
            formatted_value = "65535",
            id = 69,
            is_mfg_sp2ecific = false,
            name = "KeypadProgrammingEventMask",
            type = "bitmap16_t",
            value = "65535"
        },
        {
            cluster_id = 257,
            formatted_value = "65535",
            id = 70,
            is_mfg_sp2ecific = false,
            name = "RFProgrammingEventMask",
            type = "bitmap16_t",
            value = "65535"
        }
    },
    commands = {
    }
}
local cluster_temperature_measurement = {
    attributes = {
        {
            cluster_id = 1026,
            formatted_value = 2500,
            id = 0,
            is_mfg_sp2ecific = false,
            name = "MeasuredValue",
            type = "int16_t",
            value = "2500"
        },
        {
            cluster_id = 1026,
            formatted_value = 0,
            id = 1,
            is_mfg_sp2ecific = false,
            name = "MinMeasuredValue",
            type = "int16_t",
            value = "0"
        },
        {
            cluster_id = 1026,
            formatted_value = 6000,
            id = 2,
            is_mfg_sp2ecific = false,
            name = "MaxMeasuredValue",
            type = "int16_t",
            value = "6000"
        },
        {
            cluster_id = 1026,
            formatted_value = 300,
            id = 3,
            is_mfg_sp2ecific = false,
            name = "Tolerance",
            type = "uint16_t",
            value = "300"
        }
    },
    commands = {},
    data_holder = "server",
    endpoint_id = 2,
    id = 1026,
    name = "temperature_measurement"
}
local endpoint_1 = {
    id = 1,
    clusters = { cluster_door_lock, cluster_temperature_measurement}
}

local node = {
    id = 42,
    manufacturer_id = "Kwikset",
    model_id = "SMARTCODE_DEADBOLT_10",
    security = 0,
    is_reachable = 1,
    is_battery_powered = true,

    endpoints = { endpoint_1 }
}

local events_handler = assert( loadfile( "HUB:zigbee/scripts/events/zigbee/events_handling" ) )

local zigbee = require( "zigbee" )
zigbee.request_user_codes = tests.create_mock()

-- prepare zigbee
zigbee.add_node( node )

-- add node
events_handler( {
    event = "node_added",
    node_id = 42
} )

local door_lock_state = require("HUB:zigbee/scripts/resources/enums/door_lock_state")

-- check adding result
local expected_items = {
    {
        name = "door_lock",
        endpoint_id = 1,
        cluster_name = "door_lock",
        attribute_name = "LockState",
        value_type = "token",
        enum = {
            door_lock_state.unsecured,
            door_lock_state.secured,
            door_lock_state.unknown
        },
        value = door_lock_state.unknown
    }
}

local expected_temperature_items = {
    {
        name = "temp",
        endpoint_id = 1,
        cluster_name = "temperature_measurement",
        attribute_name = "MeasuredValue",
        value_type = "temperature",
        value = { value = 0, scale = "celsius" }
    }
}

local expected_temperature_settings = {
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
        name = "Door Lock",
        category = "door_lock",
        items = expected_items,
        node_id = 42,
        battery_powered = true
    },
    {
        name = "Temperature Sensor",
        category = "temperature",
        items = expected_temperature_items,
        settings = expected_temperature_settings,
        node_id = 42,
        battery_powered = true
    }
}

local check_context = {
    descriptors = {},
    item_ids = {},
    device_ids = {},
    setting_ids = {}
}

local test_suite_dir = "HUB:zigbee/scripts/tests/device_adding_removing_test_suite/"
loadfile( test_suite_dir .. "check_zigbee_node_adding_result" )( expected_devices, check_context )

tests.assert_called(
    zigbee.request_user_codes,
    "check if pin codes requested",
    { }
)

-- remove node
events_handler( {
    event = "node_removed",
    node_id = 42,
    manufacturer_id = "Kwikset",
    model_id = "SMARTCODE_DEADBOLT_10"
} )

-- check node removing result
loadfile( test_suite_dir .. "check_zigbee_node_removing_result" )( check_context )
