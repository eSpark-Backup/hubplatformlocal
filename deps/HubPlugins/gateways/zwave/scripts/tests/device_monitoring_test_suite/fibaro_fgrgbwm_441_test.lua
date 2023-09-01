local tests = require("tests")
local zwave = require("zwave")
local core = require("core")
local timer = require("timer")
local item_names = require("HUB:zwave/scripts/resources/item_names")
local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local events_handler = loadfile("HUB:zwave/scripts/events/events_handling")
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

local fixture = loadfile("HUB:zwave/scripts/tests/fixtures/fibaro_fgrgbwm_441_fixture")()

zwave.request_node_values = tests.create_mock()
zwave.request_value = tests.create_mock()

local function test_impl(region)
    local node = fixture:create_node(9000, region)

    zwave.add_node( node )

    -- add node
    events_handler( {
        event = "node_added",
        node_id = 9000 
    } )

    local function send_dimmer_value_updated(channel_id, value)
        events_handler( {
            event = "value_updated",
            node_id = 9000,
            channel_id = channel_id,
            class_id = class_ids.dimmer,
            value = value
        } )
    end

    local function send_color_value_updated(colors)
        events_handler( {
            event = "rgb_value_updated",
            node_id = 9000,
            channel_id = 0,
            colors = colors
        } )
    end

    local color_value = {
        red = 0,
        green = 0,
        blue = 0,
        wwhite = 0,
    }

    local color_components = { "red", "green", "blue", "wwhite" }

    local target_color_value = {
        red = 26,
        green = 27,
        blue = 29,
        wwhite = 32,
    }

    local target_dimmer_values = {
        red = 10,
        green = 11,
        blue = 12,
        wwhite = 13,
    }

    for channel_id = 2, 5 do
        local color_component = color_components[ channel_id - 1 ]
        local dimmer_value = target_dimmer_values[color_component]
        local color_component_value = target_color_value[color_component]

        -- dimmer updated first time
        do
            send_dimmer_value_updated(channel_id, dimmer_value)
            timer.add_time(2000)
            tests.assert_called(zwave.request_value, "request dimmer value", { { 9000, class_ids.dimmer, channel_id } })
        end

        -- resend dimmer value
        do
            send_dimmer_value_updated(channel_id, dimmer_value)
            timer.add_time(2500)
            tests.assert_called(zwave.request_value, "don't request color value immediatelly", { })
            timer.add_time(500)
            tests.assert_called(zwave.request_value, "request color value", { { 9000, class_ids.switch_color, 0 } })
        end

        -- request color value after transition done
        do
            send_dimmer_value_updated(channel_id, dimmer_value)
            timer.add_time(3000)
            tests.assert_called(zwave.request_value, "request color value", { { 9000, class_ids.switch_color, 0 } })
        end

        -- color value updated
        do
            color_value[ color_component ] = color_component_value
            send_color_value_updated(color_value)
            timer.add_time(3000)
            tests.assert_called(zwave.request_value, "check transition done: request color value again", { { 9000, class_ids.switch_color, 0 } })
        end

        -- send mathing dimmer value, don't request color
        do
            send_dimmer_value_updated(channel_id, dimmer_value)
            timer.add_time(3000)
            tests.assert_called(zwave.request_value, "don't request color", { })
        end
    end

    for channel_id = 0, 1 do
        do
            send_dimmer_value_updated(channel_id, 10)
            timer.add_time(2000)
            tests.assert_called(zwave.request_value, "request dimmer value", { { 9000, class_ids.dimmer, channel_id } })
        end

        do
            send_dimmer_value_updated(channel_id, 10)
            timer.add_time(5000)
            tests.assert_called(zwave.request_value, "don't request color value for non-color channel", { })
        end
    end

    events_handler( {
        event = "node_removed",
        node_id = 9000,   
        manufacturer_id = node.manufacturer_id,
        product_type = node.product_type,
        product_id = node.product_id
    })
end

tests.run_with_params(test_impl, 
{
    {
        value = "us",
        description = "US device"
    },
    {
        value = "eu",
        description = "EU device"
    }
})