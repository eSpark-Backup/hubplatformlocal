local fixture = require("HUB:zwave/scripts/tests/fixtures/settings_fixture")

fixture.change_node_manufacturer_id( 316, 2, 12 )

fixture.add_node()
fixture.check_add_configuration_called(
    { 2, 1 }, 
    { 3, 1 }, 
    { 4, 1 }, 
    { 5, 1 }, 
    { 6, 1 }, 
    { 7, 1 }, 
    { 8, 1 }, 
    { 9, 1 }, 
    { 10, 1 }, 
    { 11, 1 }, 
    { 12, 1 }, 
    { 13, 1 },
    { 20, 1 },
    { 21, 1 },
    { 22, 1 }
)

fixture.check_device_settings({
    fixture.rediscovering_configuration,
    fixture.generic_zwave_configurations,
    {
        label = {
            lang_tag = "zwave_config_label_316_2_12_2",
            text = "Basic Set Level"
        },
        description = {
            lang_tag = "zwave_config_descr_316_2_12_2",
            text = "Setting the BASIC command value to turn on the light. The 0xFF(-1) means turn on the light. For dimmer equipment 1 to 100 means the light strength. 0 means turn off the light."
        },
        value_type = "int",
        value_min = 0,
        value_max = 255,
        value_default = 255,
        value = 255,
        info = {
            ["zwave.parameter_number"] = 2,
            ["zwave.parameter_size"] = 1
        }
    },
    {
        label = {
            lang_tag = "zwave_config_label_316_2_12_3",
            text = "PIR Sensitivity"
        },
        description = {
            lang_tag = "zwave_config_descr_316_2_12_3",
            text = "PIR sensitivity settings. 0 means disable the PIR motion. 1 means the lowest sensitivity, 99 means the highest sensitivity. High sensitivity means can detected long distance, but if there is more noise signal in the environment, it will re-trigger too frequency."
        },
        value_type = "int",
        value_min = 0,
        value_max = 99,
        value_default = 80,
        value = 80,
        info = {
            ["zwave.parameter_number"] = 3,
            ["zwave.parameter_size"] = 1
        }
    },
    {
        label = {
            lang_tag = "zwave_config_label_316_2_12_4",
            text = "Light Threshold"
        },
        description = {
            lang_tag = "zwave_config_descr_316_2_12_4",
            text = "Setting the illumination threshold to turn on the light. When the event triggered and the environment illumination lower then the threshold, the device will turn on the light. 0 means turn off illumination detected function. And never turn on the light. 1 means darkest. 99 means brightest. 100 means turn off illumination detected function. And always turn on the light."
        },
        value_type = "int",
        value_min = 0,
        value_max = 99,
        value_default = 99,
        value = 99,
        info = {
            ["zwave.parameter_number"] = 4,
            ["zwave.parameter_size"] = 1
        }
    },
    {
        label = {
            lang_tag = "zwave_config_label_316_2_12_5",
            text = "Operation Mode"
        },
        description = {
            lang_tag = "zwave_config_descr_316_2_12_5",
            text = "Operation mode."
        },
        value_type = "array.token",
        enum = {
            test_mode = {
                text = "Test mode"
            },
            disable_door_window = {
                text = "Disable the door/window function"
            },
            temperature_scale = {
                text = "Setting the temperature scale Celsius"
            },
            disable_illumination_report = {
                text = "Disable the illumination report after event triggered"
            },
            disable_temperature_report = {
                text = "Disable the temperature report after event triggered"
            },
            disable_back_key_release = {
                text = "Disable the back key release into test mode"
            },
        },
        value_default = {},
        value = {},
        info = {
            ["zwave.parameter_number"] = 5,
            ["zwave.parameter_size"] = 1
        }
    },
    {
        label = {
            lang_tag = "zwave_config_label_316_2_12_6",
            text = "Mult-Sensor Function Switch"
        },
        description = {
            lang_tag = "zwave_config_descr_316_2_12_6",
            text = "Multisensor function switch."
        },
        value_type = "array.token",
        enum = {
            disable_magnetic = {
                text = "Disable magnetic integrate illumination to turn ON the lighting nodes in the association group 2"
            },
            disable_pir = {
                text = "Disable PIR integrate Illumination to turn ON the lighting nodes in the association group 2"
            },
            disable_magnetic_enable_pir = {
                text = "Disable magnetic integrate PIR to turn ON the lighting nodes in the association group 2"
            },
            are_lights_in_the_same_room = {
                text = "When Bit2 is 0 (Enable), Are the device and the lighting in the same room"
            },
            disable_delay_5_sec = {
                text = "Disable delay 5 seconds to turn off the light, when door/window closed"
            },
            disable_auto_turn_off = {
                text = "Disable auto turn off the light, after door/window opened to turn on the light"
            },
        },
        value_default = { "disable_magnetic_enable_pir" },
        value = { "disable_magnetic_enable_pir" },
        info = {
            ["zwave.parameter_number"] = 6,
            ["zwave.parameter_size"] = 1
        }
    },
    {
        label = {
            lang_tag = "zwave_config_label_316_2_12_7",
            text = "Customer function"
        },
        description = {
            lang_tag = "zwave_config_descr_316_2_12_7",
            text = "Customer function switch, using bit control."
        },
        value_type = "array.token",
        enum = {
            motion_off_report = {
                text = "Enable sending motion OFF report"
            },
            rip_super_sensitivity = {
                text = "Enable PIR super sensitivity mode"
            },
            basic_off = {
                text = "Disablesend out BASIC OFF after door closed"
            },
            notification_type = {
                text = "Using Sensor Binary Report"
            },
            disable_multi_cc = {
                text = "Disable Multi CC in auto report"
            },
            disable_battery_state_report = {
                text = "Disable to report battery state when the device triggered"
            },
        },
        value_default = { "motion_off_report", "rip_super_sensitivity", "notification_type" },
        value = { "motion_off_report", "rip_super_sensitivity", "notification_type" },
        info = {
            ["zwave.parameter_number"] = 7,
            ["zwave.parameter_size"] = 1
        }
    },
    {
        label = {
            lang_tag = "zwave_config_label_316_2_12_8",
            text = "PIR Re-Detect Interval Time"
        },
        description = {
            lang_tag = "zwave_config_descr_316_2_12_8",
            text = "In the normal mode, after the PIR motion detected, setting the re-detect time. 8 seconds per tick, default tick is 3 (24 seconds).Setting the suitable value to prevent received the trigger signal too frequently. Also can save the battery energy."
        },
        value_type = "int",
        value_min = 1,
        value_max = 127,
        value_default = 3,
        value = 3,
        info = {
            ["zwave.parameter_number"] = 8,
            ["zwave.parameter_size"] = 1
        }
    },
    {
        label = {
            lang_tag = "zwave_config_label_316_2_12_9",
            text = "Turn Off Light Time"
        },
        description = {
            lang_tag = "zwave_config_descr_316_2_12_9",
            text = "After turn on the lighting, setting the delay time to turn off the lighting when the PIR motion is not detected. 8 seconds per tick, default tick is 4 (32seconds). 0 means never send turn off light command."
        },
        value_type = "int",
        value_min = 0,
        value_max = 127,
        value_default = 4,
        value = 4,
        info = {
            ["zwave.parameter_number"] = 9,
            ["zwave.parameter_size"] = 1
        }
    },
    {
        label = {
            lang_tag = "zwave_config_label_316_2_12_10",
            text = "Auto Report Battery Time"
        },
        description = {
            lang_tag = "zwave_config_descr_316_2_12_10",
            text = "The interval time for auto report the battery level."
        },
        value_type = "int",
        value_min = 0,
        value_max = 127,
        value_default = 12,
        value = 12,
        info = {
            ["zwave.parameter_number"] = 10,
            ["zwave.parameter_size"] = 1
        }
    },
    {
        label = {
            lang_tag = "zwave_config_label_316_2_12_11",
            text = "Auto Report Door/Window State"
        },
        description = {
            lang_tag = "zwave_config_descr_316_2_12_11",
            text = "The interval time for auto report the door/window state."
        },
        value_type = "int",
        value_min = 0,
        value_max = 127,
        value_default = 12,
        value = 12,
        info = {
            ["zwave.parameter_number"] = 11,
            ["zwave.parameter_size"] = 1
        }
    },
    {
        label = {
            lang_tag = "zwave_config_label_316_2_12_12",
            text = "Auto Report Illumination Time"
        },
        description = {
            lang_tag = "zwave_config_descr_316_2_12_12",
            text = "The interval time for auto report the illumination."
        },
        value_type = "int",
        value_min = 0,
        value_max = 127,
        value_default = 12,
        value = 12,
        info = {
            ["zwave.parameter_number"] = 12,
            ["zwave.parameter_size"] = 1
        }
    },
    {
        label = {
            lang_tag = "zwave_config_label_316_2_12_13",
            text = "Auto Report Temperature Time"
        },
        description = {
            lang_tag = "zwave_config_descr_316_2_12_13",
            text = "The interval time for auto report the temperature."
        },
        value_type = "int",
        value_min = 0,
        value_max = 127,
        value_default = 12,
        value = 12,
        info = {
            ["zwave.parameter_number"] = 13,
            ["zwave.parameter_size"] = 1
        }
    },
    {
        label = {
            lang_tag = "zwave_config_label_316_2_12_20",
            text = "Auto Report Tick Interval"
        },
        description = {
            lang_tag = "zwave_config_descr_316_2_12_20",
            text = "The interval time for auto report each tick. Setting this configuration will effect configuration No.10, No.11, No.12 and No.13. The unit is 1 minute."
        },
        value_type = "int",
        value_min = 0,
        value_max = 255,
        value_default = 30,
        value = 30,
        info = {
            ["zwave.parameter_number"] = 20,
            ["zwave.parameter_size"] = 1
        }
    },
    {
        label = {
            lang_tag = "zwave_config_label_316_2_12_21",
            text = "Temperature Differential Report"
        },
        description = {
            lang_tag = "zwave_config_descr_316_2_12_21",
            text = "The temperature differential to report. 0 means turn off this function. The unit is Fahrenheit. Enable this function the device will detect every minutes. And when the temperature is over 140 degree Fahrenheit, it will continue report."
        },
        value_type = "int",
        value_min = 0,
        value_max = 127,
        value_default = 1,
        value = 1,
        info = {
            ["zwave.parameter_number"] = 21,
            ["zwave.parameter_size"] = 1
        }
    },
    {
        label = {
            lang_tag = "zwave_config_label_316_2_12_22",
            text = "Illumination Differential Report"
        },
        description = {
            lang_tag = "zwave_config_descr_316_2_12_22",
            text = "The illumination differential to report. 0 means turn off this function. The unit is percentage. Enable this function the device will detect every minutes."
        },
        value_type = "int",
        value_min = 0,
        value_max = 99,
        value_default = 0,
        value = 0,
        info = {
            ["zwave.parameter_number"] = 22,
            ["zwave.parameter_size"] = 1
        }
    }
})

fixture.remove_node()
