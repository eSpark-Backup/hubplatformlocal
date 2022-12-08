local item_names = require("HUB:zwave/scripts/resources/item_names")

return setmetatable( 
    { 
        name = item_names.user_codes_keypad_mode,
        has_setter = true
    }, 
    require("HUB:zwave/scripts/model/items/default/base_token_item_mt") 
)


