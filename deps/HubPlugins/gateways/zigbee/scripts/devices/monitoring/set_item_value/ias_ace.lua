local handler, item_id, descriptor, value = ...

local zigbee = require("zigbee")
local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")
local values_converters = hubplugins.weak_loadfile("HUB:zigbee/scripts/resources/values_converters/ias_ace")()

if descriptor.item_name ~= item_names.zones_status then return end

handler:update_value_in_core( item_id, value)

zigbee.send_command(
    descriptor.node_id,
    descriptor.endpoint_id,
    descriptor.cluster_name,
    "PanelStatusChanged",
    values_converters.zone_status_to_panel_status[value]
)
