local self, setting_id, value = ...

local zwave = require("zwave")
zwave.reconstruct_network()
