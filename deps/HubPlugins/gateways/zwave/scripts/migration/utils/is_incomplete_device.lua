local device = ...

local core = require("core")

return device.category == "unknown" and
       device.name == "Unknown" and
       device.device_type_id == "0_0_0" and
       not core.get_items_by_device_id( device.id )
