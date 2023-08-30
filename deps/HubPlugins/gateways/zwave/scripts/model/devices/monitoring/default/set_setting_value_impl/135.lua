local self, setting_id, value, descriptor = ...

local zwave = require("zwave")
if descriptor.tag == "identify" then
    zwave.send_identify(descriptor.node_id, descriptor.channel_id)
end
