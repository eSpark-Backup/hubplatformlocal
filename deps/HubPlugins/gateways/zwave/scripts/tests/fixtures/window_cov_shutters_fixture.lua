local fixture = {}

local base_value_mt = {
    __index = {
        icon_type = "",
        generic_type = "",
        specific_type = "",
        product_id = 1,
        product_type = 2,
        manufacturer_id = 3
    }
}

local function make_specifics(result)
    for _, specific in ipairs(result) do
        setmetatable(specific.value, base_value_mt)
    end

    return result
end

function fixture:create_node(node_id, product_info)
    local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )

    local cc_dimmer = { 
        id = class_ids.dimmer,
    }

    local channel_0 = { 
        id = 0,
        classes = { cc_dimmer }
    }

    return {
        id = node_id,
        manufacturer_id = product_info.manufacturer_id,
        product_type = product_info.product_type,
        product_id = product_info.product_id,
        is_battery_powered = true,

        specific_type = product_info.specific_type,
        generic_type = product_info.generic_type,
        icon_type = product_info.icon_type,

        security = { "none" },
        security_class_started = { "none" },

        channels = { channel_0 }
    }
end

function fixture:get_manufacturer_specifics()
    return make_specifics({
        {
            value = { 
                manufacturer_id = 134,
                product_type = 3,
                product_id = 141,
                name = "Nano Shutter"
            },
            description = "Nano Shutter: 134_3_141"
        },
        {
            value = { 
                manufacturer_id = 271,
                product_type = 768,
                product_id = 8202,
                name = "Roller Shutter"
            },
            description = "Roller Shutter: 271_768_8202"
        },
        {
            value = { 
                manufacturer_id = 271,
                product_type = 768,
                product_id = 258,
                name = "Roller Shutter"
            },
            description = "Roller Shutter: 271_768_258"
        },
        {
            value = { 
                manufacturer_id = 316,
                product_type = 1,
                product_id = 21,
                name = "Roller Shutter Controller"
            },
            description = "Roller Shutter Controller: 316_1_21"
        },
        {
            value = { 
                manufacturer_id = 345,
                product_type = 3,
                product_id = 2,
                name = "Flush Shutter"
            },
            description = "Flush Shutter: 345_3_2"
        },
        {
            value = { 
                manufacturer_id = 345,
                product_type = 3,
                product_id = 83,
                name = "Flush Shutter DC"
            },
            description = "Flush Shutter DC: 345_3_2"
        },
        {
            value = { 
                manufacturer_id = 51,
                product_type = 21072,
                product_id = 12337,
                name = "AC Motor Controller"
            },
            description = "AC Motor Controller: 51_21072_12337"
        }
    })
end

function fixture:get_autodetection_specifics()
    return make_specifics({
        {
            value = { 
                name = "Window Covering",
                generic_type = "GENERIC_TYPE_SWITCH_MULTILEVEL",
                specific_type = "SPECIFIC_TYPE_CLASS_A_MOTOR_CONTROL"
            },
            description = "window_cov autodetection by generic_type GENERIC_TYPE_SWITCH_MULTILEVEL and specific type SPECIFIC_TYPE_CLASS_A_MOTOR_CONTROL"
        },
        {
            value = { 
                name = "Window Covering",
                generic_type = "GENERIC_TYPE_SWITCH_MULTILEVEL",
                specific_type = "SPECIFIC_TYPE_MOTOR_MULTIPOSITION"
            },
            description = "window_cov autodetection by generic_type GENERIC_TYPE_SWITCH_MULTILEVEL and specific type SPECIFIC_TYPE_MOTOR_MULTIPOSITION"
        },
        {
            value = { 
                name = "Window Covering",
                generic_type = "GENERIC_TYPE_SWITCH_MULTILEVEL",
                specific_type = "SPECIFIC_TYPE_SECURE_LOCKBOX"
            },
            description = "window_cov autodetection by generic_type GENERIC_TYPE_SWITCH_MULTILEVEL and specific type SPECIFIC_TYPE_SECURE_LOCKBOX"
        },

        {
            value = { 
                name = "Window Covering",
                generic_type = "GENERIC_TYPE_WINDOW_COVERING",
                specific_type = "SPECIFIC_TYPE_SIMPLE_WINDOW_COVERING"
            },
            description = "window_cov autodetection by generic_type GENERIC_TYPE_WINDOW_COVERING and specific type SPECIFIC_TYPE_SIMPLE_WINDOW_COVERING"
        },
        {
            value = { 
                name = "Window Covering",
                generic_type = "GENERIC_TYPE_WINDOW_COVERING",
                specific_type = "SPECIFIC_TYPE_NOT_USED"
            },
            description = "window_cov autodetection by generic_type GENERIC_TYPE_WINDOW_COVERING and specific type SPECIFIC_TYPE_NOT_USED"
        }
    })
end

return fixture