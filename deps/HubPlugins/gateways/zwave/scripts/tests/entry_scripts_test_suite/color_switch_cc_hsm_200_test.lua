local zwave = require("zwave")
local core = require("core")
local timer = require("timer")
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local item_value_type = require("HUB:zwave/scripts/resources/item_value_type")
local item_names = require("HUB:zwave/scripts/resources/item_names")
local tests = require("tests")
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local set_item_value = loadfile( "HUB:zwave/scripts/set_item_value" )

zwave.request_node_values = tests.create_mock()
zwave.request_value = tests.create_mock()
zwave.set_rgb_value = tests.create_mock()
zwave.color_switch_level_change = tests.create_mock()

local channel_0 = {
    id = 0,
    classes = {
        {
            id = class_ids.switch_color,
            version = 2,
            component_ids = { "red", "green", "blue" }
        }
    }
}

local node = {
    id = "9000",
    manufacturer_id = 12,
    product_type = 4,
    product_id = 1,
    is_battery_powered = false,

    specific_type = "",
    generic_type = "",

    security = { "none" },
    security_class_started = { "none" },

    channels = { channel_0 }
}

zwave.add_node( node )

-- add node
events_handler( {
    event = "node_added",
    node_id = 9000
} )

local function get_item_id(item_name)
    return storage:call("find_item_id", item_name, 9000, class_ids.switch_color, 0 )
end

local function get_color_item_value()
    return core.get_item( get_item_id(item_names.rgb_color) ).value
end

-- unsupported component
do
    set_item_value({
        item_id = get_item_id(item_names.rgb_color),
        value = { cwhite = 50, wwhite = 255 }
    })

    tests.assert_called(zwave.set_rgb_value, "check set rgb value called", {})
    tests.assert_called(zwave.request_value, "check request value called", {})
end

local function test_impl(params)
    set_item_value({
        item_id = get_item_id(item_names.rgb_color),
        value = params.init_value
    })

    tests.assert_called(zwave.set_rgb_value, "check set rgb value called", { { "9000", class_ids.switch_color, 0, params.sended_value } })
    tests.assert_called(zwave.request_value, "check request value called", { { "9000", class_ids.switch_color, 0 } })
end

tests.run_with_params(
    test_impl,
    {
        {
            value = {
                init_value = {red = 255, green = 0, blue = 0},
                sended_value = {red = 255, green = 0, blue = 0},
            },
            description = "test red"
        },
        {
            value = {
                init_value = {red = 200, green = 30, blue = 50},
                sended_value = {red = 255, green = 0, blue = 0},
            },
            description = "test red adjust"
        },
        {
            value = {
                init_value = {red = 0, green = 255, blue = 0},
                sended_value = {red = 0, green = 255, blue = 0},
            },
            description = "test green"
        },
        {
            value = {
                init_value = {red = 40, green = 200, blue = 30},
                sended_value = {red = 0, green = 255, blue = 0},
            },
            description = "test green adjust"
        },
        {
            value = {
                init_value = {red = 0, green = 0, blue = 255},
                sended_value = {red = 0, green = 0, blue = 255},
            },
            description = "test blue"
        },
        {
            value = {
                init_value = {red = 35, green = 78, blue = 200},
                sended_value = {red = 0, green = 0, blue = 255},
            },
            description = "test blue adjust"
        },
        {
            value = {
                init_value = {red = 255, green = 0, blue = 255},
                sended_value = {red = 255, green = 0, blue = 255},
            },
            description = "test magenta"
        },
        {
            value = {
                init_value = {red = 190, green = 78, blue = 220},
                sended_value = {red = 255, green = 0, blue = 255},
            },
            description = "test magenta adjust"
        },
        {
            value = {
                init_value = {red = 255, green = 255, blue = 0},
                sended_value = {red = 255, green = 255, blue = 0},
            },
            description = "test yellow"
        },
        {
            value = {
                init_value = {red = 190, green = 180, blue = 60},
                sended_value = {red = 255, green = 255, blue = 0},
            },
            description = "test yellow adjust"
        },
        {
            value = {
                init_value = {red = 0, green = 255, blue = 255},
                sended_value = {red = 0, green = 255, blue = 255},
            },
            description = "test cyan"
        },
        {
            value = {
                init_value = {red = 55, green = 180, blue = 150},
                sended_value = {red = 0, green = 255, blue = 255},
            },
            description = "test cyan adjust"
        },
        {
            value = {
                init_value = {red = 255, green = 255, blue = 255},
                sended_value = {red = 255, green = 255, blue = 255},
            },
            description = "test white"
        },
        {
            value = {
                init_value = {red = 190, green = 180, blue = 150},
                sended_value = {red = 255, green = 255, blue = 255},
            },
            description = "test white adjust"
        }
    }
)
