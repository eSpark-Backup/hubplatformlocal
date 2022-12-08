local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/base_fixture")()

local class_ids = require("HUB:zwave/scripts/resources/class_ids")


function fixture:create_node(node_id)
    return {
        id = node_id,
        manufacturer_id = 305,
        product_type = 2,
        product_id = 3,
        specific_type = "SPECIFIC_TYPE_COLOR_TUNABLE_MULTILEVEL",
        generic_type = "SPECIFIC_TYPE_COLOR_TUNABLE_MULTILEVEL",
        security_class_started = { "none" },
        security = { "none" },
        is_battery_powered = true,
        is_reachable = false,
        channels = {
            {
                classes = {
                    { id = class_ids.basic },
                    { id = class_ids.switch_color },
                },
                id = 0,
            },
        },
    }
end


return fixture
