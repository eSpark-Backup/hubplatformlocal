local self, node_id, channel_id, class_id, v1_alarm_level = ...

print("doorlook alarm 0X82: RF Module Power Cycled, Door lock need time set")
local zwave = require("zwave")
zwave.synchronize_device_time(node_id)
