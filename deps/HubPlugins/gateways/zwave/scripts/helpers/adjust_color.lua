local color, supported_colors = ...

local function get_color_distance(a, b)
-- TODO use Lab color space to calculate distance between colors
return math.abs(a.red - b.red) + math.abs(a.green - b.green) + math.abs(a.blue - b.blue)
end

local min_distance, value_index
for index, supported_value in ipairs(supported_colors) do
    local color_distance = get_color_distance(color, supported_value)
    if not min_distance or color_distance < min_distance then
        min_distance = color_distance
        value_index = index
    end
end

return supported_colors[value_index]
