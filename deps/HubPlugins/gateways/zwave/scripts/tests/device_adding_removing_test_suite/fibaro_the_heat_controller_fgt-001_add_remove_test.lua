local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/base_fixture")()
local zwave = require("zwave")
local tests = require("tests")
local class_ids = require("HUB:zwave/scripts/resources/class_ids")

zwave.request_node_values = tests.create_mock()

local function test_impl(product_id)
    local node = {
        basic_type = 4,
        channels = {
            {
                classes = {
                    {
                        id = 32,
                        is_supported = false,
                        version = 1
                    },
                    {
                        id = 112,
                        parameters = {},
                        version = 1
                    },
                    {
                        id = 94,
                        version = 2
                    },
                    {
                        firmware_versions = {
                            "4.0",
                            "1.5"
                        },
                        hardware_version = 1,
                        id = 134,
                        library_type = "enhanced_slave",
                        version = 2,
                        zwave_protocol_version = "4.61"
                    },
                    {
                        association_groups = {
                            {
                                group_id = 1,
                                group_nodes = {
                                    {
                                        endpoint_id = 0,
                                        node_id = 1
                                    }
                                },
                                group_nodes_max_count = 1
                            }
                        },
                        association_groups_count = 1,
                        id = 142,
                        version = 3
                    },
                    {
                        id = 49,
                        sensors = {
                            {
                                scales = {
                                    "celsius"
                                },
                                sensor_type = "air_temperature"
                            }
                        },
                        version = 5
                    },
                    {
                        id = 64,
                        modes = {
                            0,
                            1
                        },
                        version = 3
                    },
                    {
                        id = 67,
                        setpoints = {
                            {
                                max_scale = 0,
                                max_value = 36.4,
                                min_scale = 0,
                                min_value = 10,
                                type = 1
                            }
                        },
                        version = 3
                    },
                    {
                        id = 152,
                        version = 1
                    },
                    {
                        id = 159,
                        version = 1
                    },
                    {
                        association_groups = {
                            {
                                commands = {
                                    {
                                        class_id = 90,
                                        command_id = 1,
                                        name = "DEVICE_RESET_LOCALLY_NOTIFICATION"
                                    },
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
                                        class_id = 64,
                                        command_id = 3,
                                        name = "THERMOSTAT_MODE_REPORT"
                                    },
                                    {
                                        class_id = 67,
                                        command_id = 3,
                                        name = "THERMOSTAT_SETPOINT_REPORT"
                                    },
                                    {
                                        class_id = 117,
                                        command_id = 3
                                    },
                                    {
                                        class_id = 112,
                                        command_id = 6
                                    },
                                    {
                                        class_id = 83,
                                        command_id = 5
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
                        },
                        association_groups_count = 1,
                        id = 89,
                        version = 2
                    },
                    {
                        id = 96,
                        version = 4
                    },
                    {
                        id = 113,
                        notifications = {
                            {
                                event_types = {
                                    "battery_is_charging",
                                    "battery_is_fully_charged",
                                    "charge_battery_soon",
                                    "charge_battery_now"
                                },
                                type = "power_management",
                                unknown_event_types = {}
                            },
                            {
                                event_types = {
                                    "system_hardware_failure"
                                },
                                type = "system",
                                unknown_event_types = {}
                            }
                        },
                        v1_supported = false,
                        version = 8
                    },
                    {
                        id = 114,
                        version = 2
                    },
                    {
                        id = 128,
                        version = 1
                    },
                    {
                        id = 129,
                        version = 1
                    },
                    {
                        firmware_ids = {
                            4881,
                            4881
                        },
                        id = 122,
                        version = 4
                    },
                    {
                        association_groups_count = 0,
                        id = 133,
                        version = 2
                    }
                },
                id = 0
            },
            {
                classes = {
                    {
                        id = 32,
                        is_supported = false,
                        version = 1
                    },
                    {
                        association_groups = {
                            {
                                group_id = 1,
                                group_nodes_max_count = 0
                            }
                        },
                        association_groups_count = 1,
                        id = 142,
                        version = 3
                    },
                    {
                        id = 64,
                        modes = {
                            0,
                            1
                        },
                        version = 3
                    },
                    {
                        id = 67,
                        setpoints = {
                            {
                                max_scale = 0,
                                max_value = 36.4,
                                min_scale = 0,
                                min_value = 10,
                                type = 1
                            }
                        },
                        version = 3
                    },
                    {
                        association_groups = {
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
                                        class_id = 64,
                                        command_id = 3,
                                        name = "THERMOSTAT_MODE_REPORT"
                                    },
                                    {
                                        class_id = 67,
                                        command_id = 3,
                                        name = "THERMOSTAT_SETPOINT_REPORT"
                                    },
                                    {
                                        class_id = 117,
                                        command_id = 3
                                    },
                                    {
                                        class_id = 83,
                                        command_id = 5
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
                        },
                        association_groups_count = 1,
                        id = 89,
                        version = 2
                    },
                    {
                        id = 113,
                        notifications = {
                            {
                                event_types = {
                                    "battery_is_charging",
                                    "battery_is_fully_charged",
                                    "charge_battery_soon",
                                    "charge_battery_now"
                                },
                                type = "power_management",
                                unknown_event_types = {}
                            },
                            {
                                event_types = {
                                    "system_hardware_failure"
                                },
                                type = "system",
                                unknown_event_types = {}
                            }
                        },
                        v1_supported = false,
                        version = 8
                    },
                    {
                        id = 128,
                        version = 1
                    },
                    {
                        id = 129,
                        version = 1
                    },
                    {
                        association_groups_count = 0,
                        id = 133,
                        version = 2
                    },
                    {
                        id = 94,
                        version = 2
                    },
                    {
                        id = 152,
                        version = 1
                    },
                    {
                        id = 159,
                        version = 1
                    }
                },
                id = 1
            },
            {
                classes = {
                    {
                        id = 32,
                        is_supported = false,
                        version = 1
                    },
                    {
                        association_groups = {
                            {
                                group_id = 1,
                                group_nodes_max_count = 0
                            }
                        },
                        association_groups_count = 1,
                        id = 142,
                        version = 3
                    },
                    {
                        id = 49,
                        sensors = {
                            {
                                scales = {
                                    "celsius"
                                },
                                sensor_type = "air_temperature"
                            }
                        },
                        version = 5
                    },
                    {
                        association_groups = {
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
                            }
                        },
                        association_groups_count = 1,
                        id = 89,
                        version = 2
                    },
                    {
                        id = 113,
                        notifications = {
                            {
                                event_types = {
                                    "replace_battery_soon",
                                    "replace_battery_now"
                                },
                                type = "power_management",
                                unknown_event_types = {}
                            }
                        },
                        v1_supported = false,
                        version = 8
                    },
                    {
                        id = 128,
                        version = 1
                    },
                    {
                        association_groups_count = 0,
                        id = 133,
                        version = 2
                    },
                    {
                        id = 94,
                        version = 2
                    },
                    {
                        id = 152,
                        version = 1
                    },
                    {
                        id = 159,
                        version = 1
                    }
                },
                id = 2
            }
        },
        generic_type = "GENERIC_TYPE_THERMOSTAT",
        icon_type = "ICON_TYPE_GENERIC_THERMOSTAT",
        id = "9000",
        is_battery_powered = true,
        is_reachable = false,
        is_rediscover_required = false,
        is_smart_include = false,
        manufacturer_id = 271,
        product_id = product_id.product_id,
        product_type = 4865,
        security = {
            "authenticated"
        },
        security_class_started = {
            "authenticated"
        },
        specific_type = "SPECIFIC_TYPE_THERMOSTAT_GENERAL_V2"
    }

    fixture:add_node(node)

    local items = {
        {
            name = "temp",
            cc_id = class_ids.multilevel_sensor,
            channel_id = 0
        },
        {
            name = "thermostat_setpoint_heating",
            cc_id = class_ids.thermostat_setpoint,
            channel_id = 0
        },
        {
            name = "thermostat_setpoint",
            cc_id = class_ids.thermostat_setpoint,
            channel_id = 0
        },
        {
            name = "thermostat_mode",
            cc_id = class_ids.thermostat_mode,
            channel_id = 0,
            value_type = "token",
            enum = {"heat", "off"},
            value = "off"
        },
        {
            name = "thermostat_setpoint_cooling",
            cc_id = class_ids.thermostat_setpoint,
            channel_id = 0
        },
        {
            name = "thermostat_energy_saving_mode",
            cc_id = class_ids.thermostat_setpoint,
            channel_id = 0
        },
        {
            name = "battery",
            cc_id = class_ids.notification,
            channel_id = 0,
            tag = "power_management"
        },
        {
            name = "battery",
            cc_id = class_ids.battery,
            channel_id = 0,
            is_merged = true
        },
        {
            name = "battery_charging_state",
            tag = "power_management",
            cc_id = class_ids.notification,
            channel_id = 0,
            enum = {"unknown", "no_charging", "in_progress"},
            value = "unknown",
            value_type = "token"
        },
        {
            name = "hw_state",
            cc_id = class_ids.notification,
            channel_id = 0,
            value_type = "token",
            enum = {"unknown", "hardware_ok", "hardware_failure"},
            value = "hardware_ok",
            tag = "system"
        }
    }

    -- check adding result
    local expected_devices = {
        {
            name = "Radiator Thermostat",
            type = "thermostat",
            category = "hvac",
            subcategory = "heater",
            items = items,
            node_id = "9000",
            battery_powered = true,
            security = "authenticated"
        }
    }

    local check_context = {
        descriptors = {},
        item_ids = {},
        device_ids = {}
    }

    local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"

    loadfile(test_suite_dir .. "check_zwave_node_adding_result")(expected_devices, check_context)

    fixture:remove_node(node)

    loadfile(test_suite_dir .. "check_zwave_node_removing_result")(check_context)
end

tests.run_with_params(
    test_impl,
    {
        {
            value = {
                product_id = 4096
            },
            description = "test with product_id = 4096"
        },
        {
            value = {
                product_id = 4097
            },
            description = "test with product_id = 4097"
        }
    }
)
