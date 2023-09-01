return {
    endpoints = {
        {
            id = 1,
            clusters = {
                {
                    id = 0,
                    attributes = {
                        {
                            id = 0,
                            cluster_id = 0,
                            formatted_value = "1",
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "ZCLVersion",
                            type = "uint8_t",
                            value = "1"
                        },
                        {
                            id = 1,
                            cluster_id = 0,
                            formatted_value = "255",
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "ApplicationVersion",
                            type = "uint8_t",
                            value = "255"
                        },
                        {
                            id = 3,
                            cluster_id = 0,
                            formatted_value = "2",
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "HWVersion",
                            type = "uint8_t",
                            value = "2"
                        },
                        {
                            id = 4,
                            cluster_id = 0,
                            formatted_value = "Yale",
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "ManufacturerName",
                            type = "string",
                            value = "Yale"
                        },
                        {
                            id = 5,
                            cluster_id = 0,
                            formatted_value = "YRD256 TSDB",
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "ModelIdentifier",
                            type = "string",
                            value = "YRD256 TSDB"
                        },
                        {
                            id = 7,
                            cluster_id = 0,
                            formatted_value = "3",
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "PowerSource",
                            type = "enum8_t",
                            value = "3"
                        }
                    },
                    commands = {},
                    data_holder = "server",
                    endpoint_id = 1,
                    name = "basic"
                },
                {
                    id = 1,
                    attributes = {
                        {
                            id = 32,
                            cluster_id = 1,
                            formatted_value = 60,
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "BatteryVoltage",
                            type = "uint8_t",
                            value = "60"
                        },
                        {
                            id = 33,
                            cluster_id = 1,
                            formatted_value = 200,
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "BatteryPercentageRemaining",
                            type = "uint8_t",
                            value = "200"
                        },
                        {
                            id = 49,
                            cluster_id = 1,
                            formatted_value = "AA",
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "BatterySize",
                            type = "enum8_t",
                            value = "3"
                        },
                        {
                            id = 51,
                            cluster_id = 1,
                            formatted_value = 4,
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "BatteryQuantity",
                            type = "uint8_t",
                            value = "4"
                        },
                        {
                            id = 53,
                            cluster_id = 1,
                            formatted_value = "7",
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "BatteryAlarmMask",
                            type = "bitmap8_t",
                            value = "7"
                        },
                        {
                            id = 54,
                            cluster_id = 1,
                            formatted_value = "38",
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "BatteryVoltageMinThreshold",
                            type = "uint8_t",
                            value = "38"
                        },
                        {
                            id = 55,
                            cluster_id = 1,
                            formatted_value = "39",
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "BatteryVoltageThreshold1",
                            type = "uint8_t",
                            value = "39"
                        },
                        {
                            id = 56,
                            cluster_id = 1,
                            formatted_value = "42",
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "BatteryVoltageThreshold2",
                            type = "uint8_t",
                            value = "42"
                        },
                        {
                            id = 57,
                            cluster_id = 1,
                            formatted_value = "44",
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "BatteryVoltageThreshold3",
                            type = "uint8_t",
                            value = "44"
                        },
                        {
                            id = 58,
                            cluster_id = 1,
                            formatted_value = "6",
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "BatteryPercentageMinThreshold",
                            type = "uint8_t",
                            value = "6"
                        },
                        {
                            id = 59,
                            cluster_id = 1,
                            formatted_value = "13",
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "BatteryPercentageThreshold1",
                            type = "uint8_t",
                            value = "13"
                        },
                        {
                            id = 60,
                            cluster_id = 1,
                            formatted_value = "20",
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "BatteryPercentageThreshold2",
                            type = "uint8_t",
                            value = "20"
                        },
                        {
                            id = 61,
                            cluster_id = 1,
                            formatted_value = "25",
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "BatteryPercentageThreshold3",
                            type = "uint8_t",
                            value = "25"
                        },
                        {
                            id = 62,
                            cluster_id = 1,
                            formatted_value = "unknown",
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "BatteryAlarmState",
                            type = "unknown",
                            value = "unknown"
                        }
                    },
                    commands = {},
                    data_holder = "server",
                    endpoint_id = 1,
                    name = "power_configuration"
                },
                {
                    id = 3,
                    attributes = {
                        {
                            id = 0,
                            cluster_id = 3,
                            formatted_value = "0",
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "unknown",
                            type = "uint16_t",
                            value = "0"
                        }
                    },
                    commands = {},
                    data_holder = "server",
                    endpoint_id = 1,
                    name = "identify"
                },
                {
                    id = 9,
                    attributes = {},
                    commands = {},
                    data_holder = "server",
                    endpoint_id = 1,
                    name = "alarms"
                },
                {
                    id = 10,
                    attributes = {
                        {
                            id = 0,
                            cluster_id = 10,
                            formatted_value = "unknown",
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "unknown",
                            type = "unknown",
                            value = "unknown"
                        },
                        {
                            id = 1,
                            cluster_id = 10,
                            formatted_value = "0",
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "unknown",
                            type = "bitmap8_t",
                            value = "0"
                        },
                        {
                            id = 2,
                            cluster_id = 10,
                            formatted_value = "unknown",
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "unknown",
                            type = "unknown",
                            value = "unknown"
                        },
                        {
                            id = 3,
                            cluster_id = 10,
                            formatted_value = "0",
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "unknown",
                            type = "uint32_t",
                            value = "0"
                        },
                        {
                            id = 4,
                            cluster_id = 10,
                            formatted_value = "0",
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "unknown",
                            type = "uint32_t",
                            value = "0"
                        },
                        {
                            id = 5,
                            cluster_id = 10,
                            formatted_value = "unknown",
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "unknown",
                            type = "unknown",
                            value = "unknown"
                        },
                        {
                            id = 6,
                            cluster_id = 10,
                            formatted_value = "255",
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "unknown",
                            type = "uint32_t",
                            value = "255"
                        },
                        {
                            id = 7,
                            cluster_id = 10,
                            formatted_value = "315622324",
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "unknown",
                            type = "uint32_t",
                            value = "315622324"
                        }
                    },
                    commands = {},
                    data_holder = "clientServer",
                    endpoint_id = 1,
                    name = "time"
                },
                {
                    id = 25,
                    attributes = {},
                    commands = {},
                    data_holder = "client",
                    endpoint_id = 1,
                    name = "unknown"
                },
                {
                    id = 32,
                    attributes = {
                        {
                            id = 0,
                            cluster_id = 32,
                            formatted_value = "14400",
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "CheckInInterval",
                            type = "uint32_t",
                            value = "14400"
                        },
                        {
                            id = 1,
                            cluster_id = 32,
                            formatted_value = "20",
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "LongPollInterval",
                            type = "uint32_t",
                            value = "20"
                        },
                        {
                            id = 2,
                            cluster_id = 32,
                            formatted_value = "2",
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "ShortPollInterval",
                            type = "uint16_t",
                            value = "2"
                        },
                        {
                            id = 3,
                            cluster_id = 32,
                            formatted_value = "40",
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "FastPollTimeout",
                            type = "uint16_t",
                            value = "40"
                        }
                    },
                    commands = {},
                    data_holder = "server",
                    endpoint_id = 1,
                    name = "poll_control"
                },
                {
                    id = 257,
                    attributes = {
                        {
                            id = 0,
                            cluster_id = 257,
                            formatted_value = "locked",
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "LockState",
                            type = "enum8_t",
                            value = "1"
                        },
                        {
                            id = 1,
                            cluster_id = 257,
                            formatted_value = "dead_bolt",
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "LockType",
                            type = "enum8_t",
                            value = "0"
                        },
                        {
                            id = 2,
                            cluster_id = 257,
                            formatted_value = "enabled",
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "ActuatorEnabled",
                            type = "bool",
                            value = "true"
                        },
                        {
                            id = 16,
                            cluster_id = 257,
                            formatted_value = "255",
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "NumberOfLogRecordsSupported",
                            type = "uint16_t",
                            value = "255"
                        },
                        {
                            id = 17,
                            cluster_id = 257,
                            formatted_value = 250,
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "NumberOfTotalUsersSupported",
                            type = "uint16_t",
                            value = "250"
                        },
                        {
                            id = 18,
                            cluster_id = 257,
                            formatted_value = 250,
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "NumberOfPINUsersSupported",
                            type = "uint16_t",
                            value = "250"
                        },
                        {
                            id = 20,
                            cluster_id = 257,
                            formatted_value = "7",
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "NumberOfWeekDaySchedulesSupportedPerUser",
                            type = "uint8_t",
                            value = "7"
                        },
                        {
                            id = 21,
                            cluster_id = 257,
                            formatted_value = "1",
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "NumberOfYearDaySchedulesSupportedPerUser",
                            type = "uint8_t",
                            value = "1"
                        },
                        {
                            id = 23,
                            cluster_id = 257,
                            formatted_value = 8,
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "MaxPINCodeLength",
                            type = "uint8_t",
                            value = "8"
                        },
                        {
                            id = 24,
                            cluster_id = 257,
                            formatted_value = 4,
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "MinPINCodeLength",
                            type = "uint8_t",
                            value = "4"
                        },
                        {
                            id = 33,
                            cluster_id = 257,
                            formatted_value = "en",
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "Language",
                            type = "string",
                            value = "en"
                        },
                        {
                            id = 35,
                            cluster_id = 257,
                            formatted_value = "0",
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "AutoRelockTime",
                            type = "uint32_t",
                            value = "0"
                        },
                        {
                            id = 36,
                            cluster_id = 257,
                            formatted_value = "1",
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "SoundVolume",
                            type = "uint8_t",
                            value = "1"
                        },
                        {
                            id = 37,
                            cluster_id = 257,
                            formatted_value = "0",
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "OperatingMode",
                            type = "enum8_t",
                            value = "0"
                        },
                        {
                            id = 41,
                            cluster_id = 257,
                            formatted_value = "true",
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "EnableOneTouchLocking",
                            type = "bool",
                            value = "true"
                        },
                        {
                            id = 42,
                            cluster_id = 257,
                            formatted_value = "false",
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "EnableInsideStatusLED",
                            type = "bool",
                            value = "false"
                        },
                        {
                            id = 43,
                            cluster_id = 257,
                            formatted_value = "false",
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "EnablePrivacyModeButton",
                            type = "bool",
                            value = "false"
                        },
                        {
                            id = 48,
                            cluster_id = 257,
                            formatted_value = "5",
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "WrongCodeEntryLimit",
                            type = "uint8_t",
                            value = "5"
                        },
                        {
                            id = 49,
                            cluster_id = 257,
                            formatted_value = "60",
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "UserCodeTemporaryDisableTime",
                            type = "uint8_t",
                            value = "60"
                        },
                        {
                            id = 50,
                            cluster_id = 257,
                            formatted_value = "true",
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "SendPINOverTheAir",
                            type = "bool",
                            value = "true"
                        },
                        {
                            id = 52,
                            cluster_id = 257,
                            formatted_value = "0",
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "SecurityLevel",
                            type = "enum8_t",
                            value = "0"
                        },
                        {
                            id = 64,
                            cluster_id = 257,
                            formatted_value = "65535",
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "AlarmMask",
                            type = "bitmap16_t",
                            value = "65535"
                        },
                        {
                            id = 65,
                            cluster_id = 257,
                            formatted_value = "65535",
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "KeypadOperationEventMask",
                            type = "bitmap16_t",
                            value = "65535"
                        },
                        {
                            id = 66,
                            cluster_id = 257,
                            formatted_value = "65535",
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "RFOperationEventMask",
                            type = "bitmap16_t",
                            value = "65535"
                        },
                        {
                            id = 67,
                            cluster_id = 257,
                            formatted_value = "65535",
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "ManualOperationEventMask",
                            type = "bitmap16_t",
                            value = "65535"
                        },
                        {
                            id = 68,
                            cluster_id = 257,
                            formatted_value = "65535",
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "RFIDOperationEventMask",
                            type = "bitmap16_t",
                            value = "65535"
                        },
                        {
                            id = 69,
                            cluster_id = 257,
                            formatted_value = "65535",
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "KeypadProgrammingEventMask",
                            type = "bitmap16_t",
                            value = "65535"
                        },
                        {
                            id = 70,
                            cluster_id = 257,
                            formatted_value = "65535",
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "RFProgrammingEventMask",
                            type = "bitmap16_t",
                            value = "65535"
                        },
                        {
                            id = 71,
                            cluster_id = 257,
                            formatted_value = "65535",
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "RFIDProgrammingEventMask",
                            type = "bitmap16_t",
                            value = "65535"
                        },
                        {
                            id = 128,
                            cluster_id = 257,
                            formatted_value = "false",
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "unknown",
                            type = "bool",
                            value = "false"
                        }
                    },
                    commands = {},
                    data_holder = "server",
                    endpoint_id = 1,
                    name = "door_lock"
                },
                {
                    id = 2821,
                    attributes = {
                        {
                            id = 283,
                            cluster_id = 2821,
                            formatted_value = "0",
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "unknown",
                            type = "uint16_t",
                            value = "0"
                        },
                        {
                            id = 284,
                            cluster_id = 2821,
                            formatted_value = "255",
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "unknown",
                            type = "uint8_t",
                            value = "255"
                        },
                        {
                            id = 285,
                            cluster_id = 2821,
                            formatted_value = "-39",
                            is_mfg_specific = false,
                            mfg_code = 65535,
                            name = "unknown",
                            type = "int8_t",
                            value = "-39"
                        }
                    },
                    commands = {},
                    data_holder = "server",
                    endpoint_id = 1,
                    name = "diagnostics"
                }
            },
            device_id = 10,
            profile_id = 260
        }
    },
    id = 9000,
    is_battery_powered = true,
    is_reachable = false,
    last_update_time = 1636714039,
    manufacturer_id = "Yale",
    model_id = "YRD256 TSDB",
    reachable_state_timeout = 3600,
    security = false
}
