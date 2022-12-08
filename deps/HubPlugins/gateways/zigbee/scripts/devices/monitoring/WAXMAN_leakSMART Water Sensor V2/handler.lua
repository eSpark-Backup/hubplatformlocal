local handler = hubplugins.weak_loadfile("HUB:zigbee/scripts/devices/monitoring/default/handler")()

function handler.alert_received(self, node, alert_id, category, cluster_name, command_id, command_name, endpoint_id, manufacturer_specific_bits, presence_recovery)
    local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")

    if alert_id == 129 then
        local item_id = handler:find_item_id(
            item_names.water_leak_alarm,
            node.id,
            endpoint_id,
            cluster_name,
            "")

        assert(item_id ~= nil, "Try to find item id")

        handler:update_value_in_core(item_id, presence_recovery == "presence" and "water_leak_detected" or "no_water_leak")
    end
end

function handler.empty_alerts_list_received(self, node, cluster_name, command_id, command_name, endpoint_id)
    local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")

    local item_id = handler:find_item_id(
        item_names.water_leak_alarm,
        node.id,
        endpoint_id,
        cluster_name,
        "")

    assert(item_id ~= nil, "Try to find item id")

    handler:update_value_in_core(item_id, "no_water_leak")
end

return handler
