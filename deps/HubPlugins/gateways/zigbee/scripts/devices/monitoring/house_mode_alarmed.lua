local handler, item_id, descriptor, house_mode_id = ...

local zigbee = require("zigbee")

if house_mode_id == "2" then -- send only when hub is in away mode
-- send status chenged to keypad
zigbee.send_command( 
    descriptor.node_id, 
    descriptor.endpoint_id, 
    descriptor.cluster_name, 
    "PanelStatusChanged", 
    {panel_status = "entry_delay", seconds_remaining = 30}
)
end
