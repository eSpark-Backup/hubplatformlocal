local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/base_fixture")()

local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )

-- static variables
fixture.node_id = 9000
fixture.node_id_str = "9000"


function fixture:create_node()
    local channel_0 = {
        id = 0,
        classes = {
            -- { id = class_ids.configuration },
            { id = 0x85 },
            { id = 0x86 },
            { id = 0x72 },
            { id = 0x84 },
            { id = class_ids.battery },
            {
                id = class_ids.sensor_binary,
                sensor_types = nil
            },
            {
                id = class_ids.notification,
                notifications = nil
            },
        }
    }

    return {
        id = self.node_id_str,
        manufacturer_id = 134,
        product_type = 2,
        product_id = 29,
        is_battery_powered = true,

        specific_type = "SPECIFIC_TYPE_ROUTING_SENSOR_BINARY",
        generic_type = "GENERIC_TYPE_SENSOR_BINARY",

        security = { "none" },
        security_class_started = { "none" },

        channels = { channel_0 }
    }
end


return fixture
