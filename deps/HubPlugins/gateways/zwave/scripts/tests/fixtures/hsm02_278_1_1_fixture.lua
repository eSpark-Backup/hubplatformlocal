local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/base_fixture")()

local class_ids = require("HUB:zwave/scripts/resources/class_ids")


function fixture:create_node(node_id)
    return {
        id = node_id,
        manufacturer_id = 278,
        product_type = 1,
        product_id = 1,
        specific_type = "SPECIFIC_TYPE_ROUTING_SENSOR_BINARY",
        generic_type = "GENERIC_TYPE_SENSOR_BINARY",
        security_class_started = { "none" },
        security = { "none" },
        is_battery_powered = true,
        is_reachable = false,
        channels = {
            {
                classes = {
                    { id = 134 },
                    { id = 114 },
                    { id = 133 },
                    { id = 132 },
                    -- { id = class_ids.configuration },
                    { id = class_ids.basic },
                    { id = class_ids.battery },
                    {
                        id = class_ids.sensor_binary,
                        sensor_types = {},
                    },
                    {
                        v1_supported = true,
                        id = class_ids.notification,
                        notifications = {},
                    },
                },
                id = 0,
            },
        },
    }
end


return fixture
