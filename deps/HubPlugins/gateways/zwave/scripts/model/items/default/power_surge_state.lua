local item_names = require("HUB:zwave/scripts/resources/item_names")

return setmetatable( 
    { 
        name = item_names.power_surge_state,
        enum = { "no_surge" },
        value = "no_surge",
    }, 
    require("HUB:zwave/scripts/model/items/default/base_token_item_mt") 
)
