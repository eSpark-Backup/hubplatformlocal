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

local function make_specifics(result)
    for _, specific in ipairs(result) do
        setmetatable(specific.value, base_value_mt)
    end

    return result
end

function fixture:get_manufacturer_specifics()
    return make_specifics({
        {
            value = {
                manufacturer_id = 351,
                product_type = 54545,
                product_id = 20737,
                generic_type = "GENERIC_TYPE_SWITCH_MULTILEVEL",
                specific_type = "SPECIFIC_TYPE_POWER_SWITCH_MULTILEVEL",
                icon_type = "ICON_TYPE_GENERIC_LIGHT_DIMMER_SWITCH",
            },
            description = "Touch Panel Dimmer MH-DT511: 351_54545_20737"
        },
    })
end

function fixture:get_autodetection_specifics()
    return make_specifics({
        {
            value = {
                generic_type = "GENERIC_TYPE_SWITCH_MULTILEVEL",
                specific_type = "SPECIFIC_TYPE_POWER_SWITCH_MULTILEVEL",
            },
            description = "dimmable_light autodetection by generic_type GENERIC_TYPE_SWITCH_MULTILEVEL and specific type SPECIFIC_TYPE_POWER_SWITCH_MULTILEVEL"
        },
        {
            value = {
                generic_type = "GENERIC_TYPE_SWITCH_MULTILEVEL",
                specific_type = "SPECIFIC_TYPE_NOT_USED"
            },
            description = "dimmable_light autodetection by generic_type GENERIC_TYPE_SWITCH_MULTILEVEL and specific type SPECIFIC_TYPE_NOT_USED"
        },
    })
end

return fixture
