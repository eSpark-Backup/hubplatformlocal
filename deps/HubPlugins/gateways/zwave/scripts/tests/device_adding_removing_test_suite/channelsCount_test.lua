local zwave = require("zwave")
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local tests = require "tests"
local core = require( "core" )

zwave.request_node_values = tests.create_mock()

local cc_switch = { 
    id = class_ids.switch_binary,
}

local cc_version = {
    id = class_ids.version,
    zwave_protocol_version = "1.0",
    hardware_version = 1,
    firmware_versions = { "1.0" }
}

local cc_firmware_update = {
    id = class_ids.firmware_update,
    firmware_ids = { 1 }
}

local cc_sensor_multilevel = {
    id = class_ids.multilevel_sensor,
    sensors = {
        {scales = "watt", sensor_type = "power"}
    }
}

local channel_0 = { 
    id = 0,
    classes = { cc_switch, cc_sensor_multilevel, cc_version, cc_firmware_update }
}

local channel_1 = { 
    id = 1,
    classes = { cc_switch }
}


local channel_2 = { 
    id = 2,
    classes = { cc_switch }
}

local channel_4 = {
    id = 0,
    classes = { 
        {
            id = class_ids.door_lock,
            version = 2
        }
    }
}

local channel_3 =        {classes = {
    { id = 32, is_supported = true, version = 1},
    { id = 112, parameters = {}, version = 1},
    { id = 94, version = 2},
    { firmware_versions = {"1.7"}, hardware_version = 100, id = 134, library_type = "enhanced_slave", version = 2, zwave_protocol_version = "4.5" },
    {id = 114,version = 2},
    {id = 132, version = 2 },
    {id = 128, version = 1 },
    { id = 48, sensor_types = {}, version = 1 },
    {id = 49,sensors = {
        {scales = {"celsius","fahrenheit"},sensor_type = "air_temperature"},
        {scales = {"lux"},sensor_type = "illuminance"},
        {scales = {"percent"},sensor_type = "humidity"},
        {scales = {"uv_index"},sensor_type = "ultraviolet"}},
        version = 5},
    {firmware_ids = {0},id = 122,version = 2}},id = 0
}

local nodes = {}
nodes["1"] = {
    id = "1",
    manufacturer_id = 999,
    product_type = 0,
    product_id = 0,
    is_battery_powered = false,

    specific_type = "SPECIFIC_TYPE_POWER_SWITCH_BINARY",
    generic_type = "GENERIC_TYPE_SWITCH_BINARY",

    security = { "s0" },
    security_class_started = { "s0" },

    channels = { channel_0, channel_1, channel_2 }
}


nodes["2"] = {
    id = "2",
    manufacturer_id = 134,
    product_id = 100,
    product_type = 258,
    generic_type = "GENERIC_TYPE_SENSOR_MULTILEVEL",
    basic_type = 4,
    specific_type = "SPECIFIC_TYPE_ROUTING_SENSOR_MULTILEVEL",
    security = { "none" },
    security_class_started = { "none" },
    is_reachable = true,
    is_battery_powered = false,
    is_smart_include = false,
    icon_type = "ICON_TYPE_SPECIFIC_SENSOR_NOTIFICATION_MOTION_ALARM",
    wake_up_interval = 3600,
    is_rediscover_required = false,
    channels = { channel_3
    }
}

nodes["3"] = {
    id = "3",
    manufacturer_id = 0,
    product_type = 1,
    product_id = 2,
    is_battery_powered = true,

    specific_type = "",
    generic_type = "",

    security = { "none" },
    security_class_started = { "none" },

    channels = { channel_4 }
}

local function add_node( node )
    zwave.add_node( node )
    events_handler( {
        event = "node_added",
        node_id = 1 
    } )
end 

local function check_devices()
    local devices = core.get_devices()
    for _, device in ipairs(devices) do
        if device.parent_device_id == "" then
            local node_id = device.info["zwave.node"]
            local ch_count = tostring( #nodes[node_id].channels)
            tests.assert_eq(
                device.info["channels.count"],
            tostring(ch_count),
            "Compare channels count"
            )
        else
            tests.assert_eq(
                device.info["channels.count"],
            nil,
            "Compare channels count"
            )
        end

    end
end

add_node(nodes["1"])
check_devices()

add_node(nodes["2"])
check_devices()

-- remove node
events_handler( {
    event = "node_removed",
    node_id = "1",
    manufacturer_id = 999,
    product_type = 0,
    product_id = 0,
} )
check_devices()

add_node(nodes["3"])
check_devices()

