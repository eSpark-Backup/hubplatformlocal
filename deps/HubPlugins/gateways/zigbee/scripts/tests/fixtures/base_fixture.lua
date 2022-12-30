local fixture = {}

local zigbee = require("zigbee")
local events_handler = assert(hubplugins.weak_loadfile("HUB:zigbee/scripts/events/zigbee/events_handling"))

function fixture:handle_event(...)
    events_handler(...)
end

function fixture:add_node(zigbee_node)
    -- prepare zigbee
    zigbee.add_node(zigbee_node)

    -- add node
    events_handler( {
        event = "node_added",
        node_id = tonumber(zigbee_node.id)
    } )
end


function fixture:remove_node(zigbee_node)
    events_handler(
        {
            event = "node_removed",
            node_id = tonumber(zigbee_node.id),
            manufacturer_id = zigbee_node.manufacturer_id,
            model_id = zigbee_node.model_id
        }
    )
end


return fixture
