--[[
Usage:

"status": [ "idle" | "broken" ]

{
    "method": "extensions.plugin.run",
    "id": "_ID_",
    "params": {
        "script": "HUB:zwave/scripts/debug/set_device_status",
        "scriptParams": {
            "device_id": "5e7470c37f000035c77b441b",
            "status": "broken"
        }
    }
}
]]

local params = ...

local core = require("core")

core.update_device_status(params.device_id, params.status)
