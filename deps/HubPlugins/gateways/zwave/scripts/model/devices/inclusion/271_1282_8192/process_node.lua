local self, zwave_node = ...

local channels = {}
for _, channel in pairs(zwave_node.channels) do
    channels[ channel.id ] = channel
end

local class_ids = require("HUB:zwave/scripts/resources/class_ids")

local function find_class(channel_id, class_id)
    assert(channels[channel_id], "cannot find channel " .. channel_id)
    for _, class in ipairs(channels[channel_id].classes) do
        if class.id == class_id then
            return class
        end
    end
end

local temperature_sensors = {{
    sensor_type = "air_temperature",
    scales = { "celsius" },
}}

local voltage_sensors = {{
    sensor_type = "voltage",
    scales = { "volt" },
}}

local has_humidity = false
for _, sensor in ipairs(find_class(0, class_ids.multilevel_sensor).sensors) do
    if sensor.sensor_type == "humidity" then
        has_humidity = true
        break
    end
end


for i = 3, 4 do
    local multisensor_class = find_class(i, class_ids.multilevel_sensor)
    if multisensor_class then
        multisensor_class.sensors = voltage_sensors
    else
        print("cannot find multilevel sensor class in channel " .. i)
    end
end

local i = 7
while channels[i] do
    local multisensor_class = find_class(i, class_ids.multilevel_sensor)
    if multisensor_class then
        if i == 9 and has_humidity then
            multisensor_class.sensors = {{
                sensor_type = "humidity",
                scales = { "percent" },
            }}
        else
            multisensor_class.sensors = temperature_sensors
        end
    else
        print("cannot find multilevel sensor class in channel " .. i)
    end
    i = i + 1
end

local inclusion_context = self:_super_call("default", "process_node", zwave_node)

local tree_model = inclusion_context.device_tree_model

local root = tree_model:call("get_root_node")

for i = 1, 2 do
    local device_data = tree_model:call("get_data", tree_model:call("get_child", root, 4 + i))
    device_data.info.name = "Switch " .. i
    device_data.info.category = "switch"
    device_data.info.subcategory = "relay"
    device_data.info.type = "switch.relay"
end

for i = 1, 2 do
    local device_data = tree_model:call("get_data", tree_model:call("get_child", root, i))
    device_data.info.name = "Generic Sensor " .. i
    device_data.info.category = "generic_sensor"
end

for i = 1, 2 do
    local device_data = tree_model:call("get_data", tree_model:call("get_child", root, 2 + i))
    device_data.info.name = "Voltage Level Sensor " .. i
    device_data.info.category = "voltage_sensor"
end

return inclusion_context
