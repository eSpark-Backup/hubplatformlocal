local zwave = require("zwave")

hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/for_each_zwave_node_info")(function(zwave_node_info)
    local dev_handler_id = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/create_device_type_id")(
        zwave_node_info.manufacturer_id,
        zwave_node_info.product_type,
        zwave_node_info.product_id
    )

    local device_info = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_info")(dev_handler_id)

    zwave.request_node_values(
        tonumber(zwave_node_info.id), 
        device_info.request_value_delay,
        hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_unset_pin_codes_count_to_stop_requesting")(device_info) 
    )
end)
