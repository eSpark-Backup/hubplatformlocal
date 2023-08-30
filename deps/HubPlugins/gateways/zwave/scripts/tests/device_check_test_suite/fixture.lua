local fixture = {}

local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local tests = require("tests")

fixture.simple_node = {
    id = "9000",
    manufacturer_id = "1",
    product_type = "2",
    product_id = "3",
    is_battery_powered = false,

    specific_type = "",
    generic_type = "",

    security = { "none" },
    security_class_started = { "none" },

    channels = {
        {
            id = 0,
            classes = {
                { id = class_ids.switch_binary },
                { id = class_ids.dimmer }
            }
        },
        {
            id = 1,
            classes = {
                { id = class_ids.switch_binary },
                { id = class_ids.dimmer }
            }
        },
        {
            id = 2,
            classes = {
                { id = class_ids.switch_binary },
                { id = class_ids.dimmer }
            }
        },
    }
}

function fixture.add_simple_device()
    local zwave = require("zwave")
    local events_handler = loadfile("HUB:zwave/scripts/events/events_handling")

    zwave.request_node_values = tests.create_mock()

    zwave.add_node(fixture.simple_node)
    events_handler({
        event = "node_added",
        node_id = "9000"
    })
end

return fixture
