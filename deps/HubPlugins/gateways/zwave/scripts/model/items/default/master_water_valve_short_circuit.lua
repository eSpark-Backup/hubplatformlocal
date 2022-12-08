local item_names = require("HUB:zwave/scripts/resources/item_names")

return setmetatable( 
    { name = item_names.master_water_valve_short_circuit }, 
    require("HUB:zwave/scripts/model/items/default/base_token_item_mt") 
)
