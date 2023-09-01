local handler, node, endpoint_id, cluster_name, command_id, command_name = ...

local zigbee = require("zigbee")

local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")
local zone_status_enum = hubplugins.weak_loadfile("HUB:zigbee/scripts/resources/enums/zone_status")()
local emergency_state_enum = hubplugins.weak_loadfile("HUB:zigbee/scripts/resources/enums/emergency_state")()

local function process_panic_emergency_command()
    local emergency_state_item_id = handler:find_item_id(
        item_names.emergency_state,
        node.id,
        endpoint_id,
        cluster_name,
        "" )
    assert(emergency_state_item_id, "No emergency_state item")

    local new_value
    if command_name == "Panic" then
        new_value = emergency_state_enum.panic
    elseif command_name == "Emergency" then
        new_value = emergency_state_enum.emergency
    end

    assert(new_value)

    handler:update_value_in_core( emergency_state_item_id, new_value)
    handler:update_value_in_core( emergency_state_item_id, emergency_state_enum.idle)
end

local function process_get_panel_status_command()
    local zone_status_item = handler:find_item(
        item_names.zones_status,
        node.id,
        endpoint_id,
        cluster_name,
        "" )
    assert(zone_status_item)

    local zone_status_to_panel_status = hubplugins.weak_loadfile("HUB:zigbee/scripts/resources/values_converters/ias_ace")().zone_status_to_panel_status

    zigbee.send_command(
        node.id,
        endpoint_id,
        cluster_name,
        "GetPanelStatusResponse",
        zone_status_to_panel_status[zone_status_item.value]
    )
end

if command_name == "Panic" or command_name == "Emergency" then
    process_panic_emergency_command()
elseif command_name == "GetPanelStatus" then
    process_get_panel_status_command()
end
