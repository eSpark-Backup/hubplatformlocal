local base_token_item_handler_mt = require("HUB:zwave/scripts/resources/notifications/item_handlers/base_token_item_handler_mt")
local maintenance_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/maintenance_event")

return setmetatable( {
                can_be_reset_by_idle_event = true, 
                value_after_reset = maintenance_enum.ok,
            }, 
            base_token_item_handler_mt )