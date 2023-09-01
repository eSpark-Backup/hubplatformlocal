local item_names = require("HUB:zwave/scripts/resources/item_names")

return setmetatable( 
    { name = item_names.lock_operation }, 
    require("HUB:zwave/scripts/model/items/default/base_token_item_mt") 
)
