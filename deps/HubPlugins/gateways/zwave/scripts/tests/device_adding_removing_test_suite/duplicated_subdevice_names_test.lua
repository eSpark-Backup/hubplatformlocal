local tests = require( "tests" )

local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )


local channel_0 = {
    id = 0,
    classes = {
        { id = class_ids.switch_binary }
    }
}

local channel_1 = {
    id = 1,
    classes = {
        {
            id = class_ids.multilevel_sensor,
            sensors = {{
                sensor_type = "loudness"
            }}
        },
    }
}

local channel_2 = {
    id = 2,
    classes = {
        { id = class_ids.switch_binary }
    }
}

local channel_3 = {
    id = 3,
    classes = {
        { id = class_ids.switch_binary }
    }
}

local channel_4 = {
    id = 4,
    classes = {
        {
            id = class_ids.multilevel_sensor,
            sensors = {{
                sensor_type = "loudness"
            }}
        },
    }
}


local node = {
    id = "42",
    manufacturer_id = 0,
    product_type = 1,
    product_id = 2,
    is_battery_powered = true,

    specific_type = "",
    generic_type = "",

    security = { "none" },
    security_class_started = { "none" },

    channels = { channel_0, channel_1, channel_2, channel_3, channel_4 }
}

local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )

local zwave = require( "zwave" )
zwave.request_node_values = tests.create_mock()

-- prepare zwave
zwave.add_node( node )

-- add node
events_handler( {
    event = "node_added",
    node_id = 42
} )

-- check adding result


local expected_devices = {
    {
        name = "Plug",
        category = "switch",
        subcategory = "interior_plugin",
        items = {
            {
                name = "switch",
                cc_id = class_ids.switch_binary,
                channel_id = 0,
                value_type = "bool"
            }
        },
        node_id = "42",
        battery_powered = true,

        subdevices = {
            {
                name = "Plug 1",
                category = "switch",
                subcategory = "interior_plugin",
                items = {
                    {
                        name = "switch",
                        cc_id = class_ids.switch_binary,
                        channel_id = 2,
                        value_type = "bool"
                    }
                },
                node_id = "42",
                battery_powered = true
            },
            {
                name = "Plug 2",
                category = "switch",
                subcategory = "interior_plugin",
                items = {
                    {
                        name = "switch",
                        cc_id = class_ids.switch_binary,
                        channel_id = 3,
                        value_type = "bool"
                    }
                },
                node_id = "42",
                battery_powered = true
            },
            {
                name = "Loudness Sensor 1",
                category = "level_sensor",
                subcategory = "sound",
                items = {
                    {
                        name = "loudness",
                        cc_id = class_ids.multilevel_sensor,
                        channel_id = 1
                    }
                },
                node_id = "42",
                battery_powered = true
            },
            {
                name = "Loudness Sensor 2",
                category = "level_sensor",
                subcategory = "sound",
                items = {
                    {
                        name = "loudness",
                        cc_id = class_ids.multilevel_sensor,
                        channel_id = 4
                    }
                },
                node_id = "42",
                battery_powered = true
            }
        }
    }
}




local check_context = {
    descriptors = {},
    item_ids = {},
    device_ids = {}
}

local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"

loadfile( test_suite_dir .. "check_zwave_node_adding_result" )( expected_devices, check_context )

-- remove node
events_handler( {
    event = "node_removed",
    node_id = "42",
    manufacturer_id = 0,
    product_type = 1,
    product_id = 2,
} )

-- check node removing result
loadfile( test_suite_dir .. "check_zwave_node_removing_result" )( check_context )

