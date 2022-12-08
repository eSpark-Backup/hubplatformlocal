local zwave = require "zwave"
if not zwave then
    print( "Fail to load the zwave functionality" )
    return
end

hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/for_each_zwave_node_info")(
    function (node_info)
        hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/stop_daemon")(
                node_info.node_id, node_info.manufacturer_id, node_info.product_type, node_info.product_id)
    end
)

zwave.unsubscribe( "HUB:zwave/scripts/events/events_handling" )
zwave.unsubscribe( "HUB:zwave/scripts/model/gateway/events_handling/events_handling" )
