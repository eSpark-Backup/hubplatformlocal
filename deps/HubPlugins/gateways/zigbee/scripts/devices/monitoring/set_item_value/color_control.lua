local handler, item_id, descriptor, value = ...

local zigbee = require("zigbee")

local function set_color()
    local color_converter = hubplugins.object("color_converter", "HUB:zigbee/scripts/utils")

    local xyY_value = color_converter:call(
        "RGB_to_xyY",
        {
            R = value.red,
            G = value.green,
            B = value.blue
        }
    )

    zigbee.send_command(
        descriptor.node_id,
        descriptor.endpoint_id,
        descriptor.cluster_name,
        "MoveToColor",
        {
            current_x = math.floor(xyY_value.x * 65536),
            current_y = math.floor(xyY_value.y * 65536)
        }
    )

    handler:request_item_value_with_timeout(
        descriptor.node_id,
        descriptor.endpoint_id,
        descriptor.cluster_name,
        "CurrentX"
    )

    handler:request_item_value_with_timeout(
        descriptor.node_id,
        descriptor.endpoint_id,
        descriptor.cluster_name,
        "CurrentY"
    )
end

local function set_color_temperature()
    local node = zigbee.get_node(descriptor.node_id)
    local min_attr = handler:find_attribute(node, descriptor.endpoint_id, "color_control", "ColorTempPhysicalMinMireds")
    local max_attr = handler:find_attribute(node, descriptor.endpoint_id, "color_control", "ColorTempPhysicalMaxMireds")

    local min_value = min_attr and min_attr.formatted_value or 1
    local max_value = max_attr and max_attr.formatted_value or 65279

    local white_point = (max_value + min_value) / 2
    local division = (max_value - min_value) / 510

    local temperature_value = white_point
    if value.cwhite ~= 0 then
        temperature_value = white_point - (value.cwhite * division)
    else
        temperature_value = white_point + (value.wwhite * division)
    end

    zigbee.send_command(
        descriptor.node_id,
        descriptor.endpoint_id,
        descriptor.cluster_name,
        "MoveToColorTemperature",
        math.floor(temperature_value)
    )

    handler:request_item_value_with_timeout(
        descriptor.node_id,
        descriptor.endpoint_id,
        descriptor.cluster_name,
        "ColorTemperatureMireds"
    )
end


-- main


if value.red and value.green and value.blue then
    if value.red ~= 0 or value.green ~= 0 or value.blue ~= 0 then
        set_color()
        return
    end
end

if value.cwhite and value.wwhite then
    if value.cwhite ~= 0 or value.wwhite ~= 0 then
        set_color_temperature()
        return
    end
end
