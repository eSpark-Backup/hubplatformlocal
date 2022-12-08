local params = ...

local core = require("core")

core.send_ui_broadcast({
    event = params.action,
    status = params.status
})
