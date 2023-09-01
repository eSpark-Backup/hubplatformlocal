local cloud_devices = {
    devices = {}
}

local subscriptions = {
    records = {}
}

function cloud_devices.add_device( device )
    assert( device )
    cloud_devices.devices[ device.id ] = device
end

function cloud_devices.remove_device( device_id )
    assert( device_id )
    cloud_devices.devices[ device_id ] = nil
end

function cloud_devices.get_all_devices_ids()
    local result = {}
    for _, device in pairs( cloud_devices.devices ) do
        table.insert( result, device.id )
    end
    return result
end

function cloud_devices.get_device( device_id )
    return cloud_devices.devices[ device_id ]
end

function cloud_devices.send_command( command_id, device_id, capability, command, params )
    error( "Please, override cloud_devices.send_command" )
end

function cloud_devices.subscribe( script_name )
end

function cloud_devices.unsubscribe( script_name )
end

function cloud_devices.store_subscription_record(record)
   subscription_record = record.subscriptionRecord
   id = subscription_record._id
   subscription_record._id = id
   subscriptions.records[id] = subscription_record
   return
end

function cloud_devices.get_subscription_record(id)
   return subscriptions.records[id]
end

function cloud_devices.remove_subscription_record(record)
   id = record.abstractUUID
   cloud_devices.devices[ id ] = nil
end

return cloud_devices