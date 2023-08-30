local zwave = require( "zwave" )
local tests = require( "tests" )
local core = require( "core" )

local events_handler = hubplugins.weak_loadfile( "HUB:zwave/scripts/events/events_handling" )
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )

local fixture = {}

function fixture.generate_device(self, node_id, device_manufacturer_specific)
    local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )

    local cc_dimmer = {
        id = class_ids.dimmer,
    }

    local channel_0 = {
        id = 0,
        classes = { cc_dimmer }
    }

    local node = {
        id = tostring(node_id),
        manufacturer_id = device_manufacturer_specific.manufacturer_id,
        product_type = device_manufacturer_specific.product_type,
        product_id = device_manufacturer_specific.product_id,
        is_battery_powered = true,

        generic_type = "GENERIC_TYPE_WINDOW_COVERING",
        specific_type = "SPECIFIC_TYPE_SIMPLE_WINDOW_COVERING",

        security = { "none" },
        security_class_started = { "none" },

        channels = { channel_0 }
    }

    -- prepare zwave
    zwave.add_node(node)

    -- add node
    events_handler( {
        event = "node_added",
        node_id = node_id
    } )

    return storage:call("find_device_id", node_id)
end

function fixture.remove_node(self, node_id, device_manufacturer_specific)
    events_handler( {
        event = "node_removed",
        node_id = tostring(node_id),
        manufacturer_id = device_manufacturer_specific.manufacturer_id,
        product_type = device_manufacturer_specific.product_type,
        product_id = device_manufacturer_specific.product_id
    } )
end

function fixture.get_dual_shutters_manufacturer_specifics(self)
    return {
        {
            value = {
                manufacturer_id = 622,
                product_type = 21075,
                product_id = 23089,
                name = "Roller Shade Radio Z-Wave"
            },
            description = "Test with 622_21075_23089"
        }
    }
end

function fixture.find_shutter_second_part_item_id(self, node_id)
    return assert( storage:call(
        "find_item_id",
        "shutter_second_part",
        node_id,
        class_ids.dimmer,
        0
    ) )
end

function fixture.check_shutter_second_part_value(self, node_id, value)
    local item_id = self:find_shutter_second_part_item_id(node_id)

    local item = core.get_item(item_id)
    tests.assert_ne(item, nil)

    tests.assert_eq(item.value, value)
end

return fixture
