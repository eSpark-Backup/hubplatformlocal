local node_id = ...

print("start migration after rediscovering")

local zwave = require "zwave"

local changes = hubplugins.weak_loadfile("HUB:zwave/scripts/migration/version_changes/check_all")()
local zwave_node = zwave.get_node( node_id )
hubplugins.weak_loadfile("HUB:zwave/scripts/migration/process_node")( 
    zwave_node, 
    changes.items_old_names, 
    changes.storage_item_deleter, 
    changes.storage_setting_deleter, 
    changes.items_comparator,
    changes.items_to_change_value
)

local dev_handler_id = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/create_device_type_id")(
    zwave_node.manufacturer_id,
    zwave_node.product_type,
    zwave_node.product_id
)

local device_info = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_info")(dev_handler_id)
zwave.request_node_values(
    node_id, 
    device_info.request_value_delay,
    hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_unset_pin_codes_count_to_stop_requesting")(device_info) 
)
