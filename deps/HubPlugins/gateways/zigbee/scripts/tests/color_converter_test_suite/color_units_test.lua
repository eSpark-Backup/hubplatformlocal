local tests = require("tests")

local color_converter = hubplugins.object("color_converter", "HUB:zigbee/scripts/utils")

local function test(rgb, rgb_after_conversion)
    local xyY_value = color_converter:call(
        "RGB_to_xyY",
        {
            R = rgb.red,
            G = rgb.green,
            B = rgb.blue
        }
    )

    local result_rgb = color_converter:call(
        "xyY_to_RGB",
        {
            x = xyY_value.x,
            y = xyY_value.y,
            Y = 1
        }
    )

    tests.assert_eq(
        {
            red = result_rgb.R,
            green = result_rgb.G,
            blue = result_rgb.B
        },
        rgb_after_conversion
    )
end

test({red = 255, green = 0, blue = 0}, {red = 254, green = 0, blue = 0})
test({red = 0, green = 255, blue = 0}, {red = 0, green = 254, blue = 0})
test({red = 0, green = 0, blue = 255}, {red = 0, green = 0, blue = 254})

test({red = 255, green = 255, blue = 0}, {red = 254, green = 254, blue = 0})
test({red = 255, green = 0, blue = 255}, {red = 254, green = 0, blue = 254})
test({red = 0, green = 255, blue = 255}, {red = 0, green = 254, blue = 254})

test({red = 220, green = 100, blue = 50}, {red = 254, green = 117, blue = 59})
test({red = 100, green = 220, blue = 50}, {red = 117, green = 254, blue = 59})
test({red = 150, green = 100, blue = 30}, {red = 254, green = 172, blue = 58})
