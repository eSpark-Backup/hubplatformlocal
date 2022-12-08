local zigbee = {
    nodes = {},
}

function zigbee.get_node( id )
    return zigbee.nodes[ id ]
end

function zigbee.get_all_nodes_ids()
    local result = {}
    for id,_ in pairs(zigbee.nodes) do
        table.insert(result, id)
    end
    return result
end

function zigbee.add_node( node )
    zigbee.nodes[ node.id ] = node
end

function zigbee.request_attribute_value(node_id, endpoint_id, cluster_name, attribute_name)
    print( "Please, override zigbee.request_attribute_value" )
end

function zigbee.request_mfg_specific_attribute_value(node_id, endpoint_id, cluster_id, attribute_id)
    print( "Please, override zigbee.request_mfg_specific_attribute_value" )
end

function zigbee.configure_attribute_report(node_id, endpoint_id, cluster_name, attribute_name)
    print( "Please, override zigbee.configure_attribute_report" )
end

function zigbee.configure_mfg_specific_attribute_report(node_id, endpoint_id, cluster_id, attribute_id)
    print( "Please, override zigbee.configure_mfg_specific_attribute_report" )
end

function zigbee.send_command(node_id, endpoint_id, cluster_name, command_name, command_params)
    print( "Please, override zigbee.send_command" )
end

function zigbee.request_user_codes(node_id, endpoint_id, start_user_id, end_user_id)
    print( "Please, override zigbee.request_user_codes" )
end

return zigbee
