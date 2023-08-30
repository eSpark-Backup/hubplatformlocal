local handler, node, device_id, endpoint_id, cluster_name, attribute = ...

local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")
local zigbee = require("zigbee")

local function update_by_color()
    local current_x_attr = handler:find_attribute(node, endpoint_id, "color_control", "CurrentX")
    local current_y_attr = handler:find_attribute(node, endpoint_id, "color_control", "CurrentY")
    local dimmer_attr = handler:find_attribute(node, endpoint_id, "level_control", "CurrentLevel")

    assert(current_x_attr)
    assert(current_y_attr)
    assert(dimmer_attr)

    local x_value = current_x_attr.formatted_value / 65536
    local y_value = current_y_attr.formatted_value / 65536
    local Y_value = 1

    local color_converter = hubplugins.object("color_converter", "HUB:zigbee/scripts/utils")
    local rgb_value = color_converter:call(
        "xyY_to_RGB",
        {
            x = x_value,
            y = y_value,
            Y = Y_value
        }
    )

    local item = handler:find_item(item_names.rgb_color, node.id, endpoint_id, cluster_name, "")
    if item then
        local new_value = {}
        for component,_ in pairs(item.value) do
            new_value[component] = 0
        end
        new_value.red = rgb_value.R
        new_value.green = rgb_value.G
        new_value.blue = rgb_value.B

        handler:update_value_in_core(
            item.id,
            new_value
        )
    end
end

local function update_by_temperature()
    local temperature_attr = handler:find_attribute(node, endpoint_id, "color_control", "ColorTemperatureMireds")
    local min_attr = handler:find_attribute(node, endpoint_id, "color_control", "ColorTempPhysicalMinMireds")
    local max_attr = handler:find_attribute(node, endpoint_id, "color_control", "ColorTempPhysicalMaxMireds")
    assert(temperature_attr)

    local min_value = min_attr and min_attr.formatted_value or 1
    local max_value = max_attr and max_attr.formatted_value or 65279

    local white_point = (max_value + min_value) / 2
    local division = (max_value - min_value) / 510

    local diff = (white_point - temperature_attr.formatted_value) / division
    local new_value = {
        red = 0,
        green = 0,
        blue = 0,
        cwhite = 0,
        wwhite = 0
    }

    if diff < 0 then
        new_value.wwhite = math.floor(-diff)
    else
        new_value.cwhite = math.floor(diff)
    end

    local item_id = handler:find_item_id(item_names.rgb_color, node.id, endpoint_id, cluster_name, "")
    if item_id then
        handler:update_value_in_core( item_id, new_value )
    end
end


-- main


local color_mode_attr = handler:find_attribute(node, endpoint_id, "color_control", "ColorMode")
if not color_mode_attr or color_mode_attr.formatted_value == "color_xy" then
    update_by_color()
elseif color_mode_attr.formatted_value == "color_temperature" then
    update_by_temperature()
end
