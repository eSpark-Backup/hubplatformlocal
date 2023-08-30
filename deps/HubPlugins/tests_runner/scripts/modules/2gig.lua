local send_event = loadfile("HUB:2gig/scripts/events/events_handling")

local twogig = {
    devices = {}
}

function twogig.subscribe(script_name)
end

function twogig.unsubscribe(script_name)
end

function twogig.reset()
    twogig.devices = {}
end

function twogig.start_device_discovering()
end

function twogig.stop_device_discovering()
end

function twogig.add_device(device_info)
    for _, txid in ipairs(device_info.tx_ids) do
        twogig.devices[txid] = true
    end

    send_event({
        event = "device_added",
        model = device_info.model,
        tx_ids = device_info.tx_ids,
        device_type = device_info.device_type
    })
end

function twogig.remove_device(tx_ids)
    for _, txid in ipairs(tx_ids) do
        twogig.devices[txid] = nil
    end

    send_event({
        event = "device_removed",
        tx_ids = tx_ids
    })
end

function twogig.configure_device(tx_ids, config)
end

-- Note: not a real method
function twogig.get_devices()
    local txids = {}

    for txid, _ in pairs(twogig.devices) do
        table.insert(txids, txid)
    end

    return txids
end

return twogig
