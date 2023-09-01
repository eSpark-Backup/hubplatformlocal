local handler, item_id, descriptor, from, to, status, switch_delay = ...

local core = require("core")
local zigbee = require("zigbee")
local zone_status_enum = hubplugins.weak_loadfile("HUB:zigbee/scripts/resources/enums/zone_status")()
local values_converters = hubplugins.weak_loadfile("HUB:zigbee/scripts/resources/values_converters/ias_ace")()

local function should_change_zone_status(new, old)
    if new == zone_status_enum.disarmed then
        return old ~= zone_status_enum.disarmed and old ~= zone_status_enum.stay
    end

    return new ~= old
end


-- change item value
local item = core.get_item(item_id)

local new_zone_status = values_converters.house_mode_to_zone_status[status == "cancel" and from or to]
if not new_zone_status then
    print( "Unsupported house mode for keypads: " .. to )
    return
end

if should_change_zone_status(new_zone_status, item.value) then
    handler:update_value_in_core( item_id, new_zone_status)
else
    new_zone_status = item.value
end

-- send status chenged to keypad
local panel_status = values_converters.zone_status_to_panel_status[new_zone_status]
if switch_delay > 0 then
    if new_zone_status == zone_status_enum.night then
        panel_status = "arming_night"
    elseif new_zone_status == zone_status_enum.away then
        panel_status = "arming_away"
    end
end

zigbee.send_command(
    descriptor.node_id,
    descriptor.endpoint_id,
    descriptor.cluster_name,
    "PanelStatusChanged",
    {panel_status = panel_status, seconds_remaining = switch_delay}
)
