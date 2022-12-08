local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local cc_switch = { 
    id = class_ids.switch_binary,
}

local channel_0 = { 
    id = 0,
    classes = { cc_switch }
}

return {
    id = "9000",
    manufacturer_id = 132,
    product_type = 529,
    product_id = 43527,
    is_battery_powered = true,

    specific_type = "",
    generic_type = "GENERIC_TYPE_SWITCH_BINARY",

    security = { "none" },
    security_class_started = { "none" },

    channels = { channel_0 }
}
