local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local item_names = require("HUB:zwave/scripts/resources/item_names")
local tests = require("tests")
local events_handler = assert( hubplugins.weak_loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/base_fixture")()
local zwave = require("zwave")
local core = require("core")
local timer = require("timer")

zwave.request_node_values = tests.create_mock()

local original_update_value = core.update_item_value
core.update_item_value = tests.create_mock()

local node = {
    basic_type = 4,
    channels = {
        {
            classes = {
                {
                    id = 32,
                    is_supported = false,
                    version = 2
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
                    id = 37,
                    version = 1
                },
                {
                    association_groups_count = 0,
                    id = 133,
                    version = 2
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
                        },
                        {
                            group_id = 2,
                            group_nodes_max_count = 5
                        },
                        {
                            group_id = 3,
                            group_nodes_max_count = 5
                        },
                        {
                            group_id = 4,
                            group_nodes_max_count = 5
                        }
                    },
                    association_groups_count = 4,
                    id = 142,
                    version = 3
                },
                {
                    association_groups = {
                        {
                            commands = {
                                {
                                    class_id = 32,
                                    command_id = 3,
                                    name = "BASIC_REPORT"
                                },
                                {
                                    class_id = 37,
                                    command_id = 3,
                                    name = "SWITCH_BINARY_REPORT"
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
                            group_name = "BASIC SET EP 1",
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
                            group_name = "BASIC SET EP 2",
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
                            group_name = "BASIC SET EP 3",
                            profile = {
                                name = "Control:Key03",
                                profile_lsb = 3,
                                profile_msb = 32
                            }
                        }
                    },
                    association_groups_count = 4,
                    id = 89,
                    version = 1
                },
                {
                    application_build_number = 52445,
                    application_framework_api_build_number = 52445,
                    application_framework_api_version = "3.1.1",
                    application_version = "3.1.1",
                    firmware_versions = {
                        "1.4"
                    },
                    hardware_version = 2,
                    host_interface_build_number = 0,
                    id = 134,
                    library_type = "enhanced_slave",
                    protocol_build_number = 43,
                    protocol_version = "5.3.0",
                    sdk_version = "6.71.3",
                    version = 3,
                    zwave_protocol_version = "5.3"
                },
                {
                    id = 114,
                    version = 2
                },
                {
                    id = 96,
                    version = 4
                },
                {
                    id = 159,
                    version = 1
                },
                {
                    firmware_ids = {
                        40970
                    },
                    id = 122,
                    version = 4
                }
            },
            id = 0
        },
        {
            classes = {
                {
                    id = 32,
                    is_supported = false,
                    version = 2
                },
                {
                    id = 94,
                    version = 2
                },
                {
                    association_groups_count = 0,
                    id = 133,
                    version = 2
                },
                {
                    association_groups_count = 0,
                    id = 89,
                    version = 1
                },
                {
                    association_groups = {
                        {
                            group_id = 1,
                            group_nodes_max_count = 0
                        },
                        {
                            group_id = 2,
                            group_nodes_max_count = 5
                        }
                    },
                    association_groups_count = 2,
                    id = 142,
                    version = 3
                },
                {
                    id = 37,
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
                    version = 2
                },
                {
                    id = 94,
                    version = 2
                },
                {
                    association_groups_count = 0,
                    id = 133,
                    version = 2
                },
                {
                    association_groups_count = 0,
                    id = 89,
                    version = 1
                },
                {
                    association_groups = {
                        {
                            group_id = 1,
                            group_nodes_max_count = 0
                        },
                        {
                            group_id = 2,
                            group_nodes_max_count = 5
                        }
                    },
                    association_groups_count = 2,
                    id = 142,
                    version = 3
                },
                {
                    id = 37,
                    version = 1
                },
                {
                    id = 159,
                    version = 1
                }
            },
            id = 2
        },
        {
            classes = {
                {
                    id = 32,
                    is_supported = false,
                    version = 2
                },
                {
                    id = 94,
                    version = 2
                },
                {
                    association_groups_count = 0,
                    id = 133,
                    version = 2
                },
                {
                    association_groups_count = 0,
                    id = 89,
                    version = 1
                },
                {
                    association_groups = {
                        {
                            group_id = 1,
                            group_nodes_max_count = 0
                        },
                        {
                            group_id = 2,
                            group_nodes_max_count = 5
                        }
                    },
                    association_groups_count = 2,
                    id = 142,
                    version = 3
                },
                {
                    id = 37,
                    version = 1
                },
                {
                    id = 159,
                    version = 1
                }
            },
            id = 3
        }
    },
    generic_type = "GENERIC_TYPE_SWITCH_BINARY",
    icon_type = "ICON_TYPE_GENERIC_ON_OFF_POWER_SWITCH",
    id = "9000",
    is_battery_powered = false,
    is_reachable = false,
    is_rediscover_required = false,
    is_smart_include = false,
    manufacturer_id = 634,
    product_id = 40970,
    product_type = 40960,
    security = {
        "none"
    },
    security_class_started = {
        "unauthenticated"
    },
    specific_type = "SPECIFIC_TYPE_POWER_SWITCH_BINARY"
}

fixture:add_node(node)

local function send_binary_switch_updated(channel_id, value)
    events_handler({
        event = "value_updated",
        node_id = "9000",
        channel_id = channel_id,
        class_id = class_ids.switch_binary,
        value = value
    })
end

local channel_0_item_id = storage:call( "find_item_id", item_names.switch, 9000, class_ids.switch_binary, 0)
local channel_1_item_id = storage:call( "find_item_id", item_names.switch, 9000, class_ids.switch_binary, 1)
tests.assert_ne(channel_0_item_id, nil)
tests.assert_ne(channel_1_item_id, nil)

-- test same value updated 0 channel
zwave.request_value = tests.create_mock()
send_binary_switch_updated(0, 0)
timer.add_time(700)
tests.assert_called( zwave.request_value, "check no value requested",{} )

-- test same value updated 2 channel
zwave.request_value = tests.create_mock()
send_binary_switch_updated(2, 0)
timer.add_time(700)
tests.assert_called( zwave.request_value, "check no value requested",{} )

-- test another value updated, 0 channel
zwave.request_value = tests.create_mock()
core.update_item_value = tests.create_mock()
send_binary_switch_updated(0, 0xFF)
tests.assert_called( zwave.request_value, "check no value requested",{} )
tests.assert_called( core.update_item_value, "check item value updated",{ { channel_0_item_id, true } } )
timer.add_time(200)
tests.assert_called( zwave.request_value, "check 1 channel value requested",{ {"9000", class_ids.switch_binary, 1} } )
timer.add_time(200)
tests.assert_called( zwave.request_value, "check 2 channel value requested",{ {"9000", class_ids.switch_binary, 2} } )
timer.add_time(200)
tests.assert_called( zwave.request_value, "check 3 channel value requested",{ {"9000", class_ids.switch_binary, 3} } )

-- test another value updated, 1 channel
zwave.request_value = tests.create_mock()
core.update_item_value = tests.create_mock()
send_binary_switch_updated(1, 0xFF)
tests.assert_called( zwave.request_value, "check no value requested",{} )
tests.assert_called( core.update_item_value, "check item value updated",{ { channel_1_item_id, true } } )
timer.add_time(200)
tests.assert_called( zwave.request_value, "check 0 channel value requested",{ {"9000", class_ids.switch_binary, 0} } )
timer.add_time(400)
tests.assert_called( zwave.request_value, "check no value requested",{} )
