local item_names = require("HUB:zwave/scripts/resources/item_names")

return setmetatable( 
    { 
        name = item_names.door_lock,
        has_setter = true,
        enum = {
            "unsecured",
            "unknown",
            "secured"
        },
    }, 
    require("HUB:zwave/scripts/model/items/default/base_token_item_mt") 
)
