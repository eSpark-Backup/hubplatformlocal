local fixture = {}

local zwave = require("zwave")
local events_handler = assert(hubplugins.weak_loadfile("HUB:zwave/scripts/events/events_handling"))

function fixture:handle_event(...)
    events_handler(...)
end

function fixture:add_node(zwave_node)
    -- prepare zwave
    zwave.add_node(zwave_node)

    -- add node
    events_handler( {
        event = "node_added",
        node_id = tonumber(zwave_node.id)
    } )
end


function fixture:remove_node(zwave_node)
    events_handler( {
        event = "node_removed",
        node_id = tonumber(zwave_node.id),
        manufacturer_id = zwave_node.manufacturer_id,
        product_type = zwave_node.product_type,
        product_id = zwave_node.product_id
    } )
end


return fixture
