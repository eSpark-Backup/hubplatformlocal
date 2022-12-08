local core = require("core")

local gateway = core.get_gateway()
return gateway.manual_device_adding == "wizard"
