local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/base_fixture")()

local class_ids = require("HUB:zwave/scripts/resources/class_ids")


function fixture:create_node(node_id)
    local channel_0 = {
        id = 0,
        classes = {
            { id = class_ids.dimmer }
        }
    }

    local channel_1 = {
        id = 1,
        classes = {
            { id = class_ids.dimmer }
        }
    }

    local channel_2 = {
        id = 2,
        classes = {
            { id = class_ids.dimmer }
        }
    }

    return {
        id = node_id,
        manufacturer_id = 99,
        product_type = 20548,
        product_id = 12594,
        is_battery_powered = false,

        specific_type = "",
        generic_type = "",

        security = { "none" },
        security_class_started = { "none" },

        channels = { channel_0, channel_1, channel_2 }
    }
end


return fixture