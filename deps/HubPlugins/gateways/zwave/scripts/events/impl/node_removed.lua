local params = ...

-- TODO: Save node id as number and don't convert to string
local node_id = tostring(math.floor(params.node_id))
local manufacturer_id = params.manufacturer_id
local product_type = params.product_type
local product_id = params.product_id

do
    hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/stop_daemon")(node_id, manufacturer_id, product_type, product_id)
end

do
    print("Trying to remove node with id: " .. node_id)

    local dev_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_handler_exclusion")(manufacturer_id, product_type, product_id)

    dev_handler:call("remove_node", node_id)
end
