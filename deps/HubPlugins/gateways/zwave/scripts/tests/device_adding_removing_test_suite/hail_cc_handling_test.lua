local tests = require( "tests" )

local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )

local zwave = require( "zwave" )
zwave.request_value = tests.create_mock()
zwave.request_node_values = tests.create_mock()

local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )

local function add_node( node_id, manufacturer_id, product_type, product_id )
    local channel_0 = { 
        id = 0,
        classes = {
            { id = class_ids.hail },
            { id = class_ids.switch_binary },
            { id = class_ids.basic },
            { id = class_ids.battery },
        }
    }

    local channel_1 = { 
        id = 1,
        classes = {
            { id = class_ids.basic },
            { 
                id = class_ids.sensor_binary,
                sensor_types = { 
                    0x03, -- co_alarm
                    0x05, -- heat_alarm
                }
            },
            {
                id = class_ids.switch_color,
                component_ids = {}
            }
        }
    }

    local node = {
        id = node_id,
        manufacturer_id = manufacturer_id,
        product_type = product_type,
        product_id = product_id,
        is_battery_powered = true,

        specific_type = "",
        generic_type = "",
        
        security = { "none" },
        security_class_started = { "none" },

        channels = { channel_0, channel_1 }
    }

    -- prepare zwave
    zwave.add_node( node )

    -- add node
    events_handler( {
        event = "node_added",
        node_id = node_id 
    } )
end

local function test_impl( test_data )
    add_node( "1", test_data.manufacturer_id, test_data.product_type, test_data.product_id )
    add_node( "42", test_data.manufacturer_id, test_data.product_type, test_data.product_id )
    add_node( "100500", test_data.manufacturer_id, test_data.product_type, test_data.product_id )

    tests.assert_called( zwave.request_node_values, "check number of calls  ", nil, 3 )

    -- hail
    events_handler( {
        event = "hail",
        node_id = "42"
    } )

    for _, expected_hail_params in ipairs( test_data.expected_hails_params ) do
        table.insert( expected_hail_params, 1, "42" )
    end

    tests.assert_called( zwave.request_value, "check values requested", test_data.expected_hails_params )
end

tests.run_with_params( test_impl, 
{
    {
        value = {
            manufacturer_id = 0,
            product_type = 1,
            product_id = 2,
            expected_hails_params = {
                { class_ids.switch_binary, 0 },
                { class_ids.battery, 0 },
                { class_ids.sensor_binary, 1 },
                { class_ids.switch_color, 1 }
            }
        },
        description = "check general device"
    },
    {
        value = {
            manufacturer_id = 29,
            product_type = 13313,
            product_id = 1,
            expected_hails_params = {
                { class_ids.switch_binary, 0 }
            }
        },
        description = "check device 29_13313_1"
    }
} )