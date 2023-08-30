local tests = require( "tests" )
local core = require( "core" )

local old_loadfile = loadfile
loadfile = function(path)
    if path == "HUB:zigbee/scripts/devices/device_info/generated/Ezlo_ComboSensor" then
        return function()
            local dev_info = old_loadfile("HUB:zigbee/scripts/devices/device_info/default")()
            dev_info.info = {manufacturer = "EzloTest", model = "ComboSensorTest"}
            return dev_info
        end
    else
        return old_loadfile(path)
    end
end

local function cluster_ias_zone_generator(zone_type)
    return { 
        name = "ias_zone",
        attributes = {
            {
                name = "ZoneType",
                formatted_value = {zone_type = zone_type}
            }
        }
    }
end

local endpoint_1 = { 
    id = 1,
    clusters = { cluster_ias_zone_generator("motion_sensor") }
}

local endpoint_2 = { 
    id = 2,
    clusters = { cluster_ias_zone_generator("fire_sensor") }
}

local endpoint_3 = { 
    id = 3,
    clusters = { cluster_ias_zone_generator("gas_sensor") }
}

local node = {
    id = 42,
    manufacturer_id = "Ezlo",
    model_id = "ComboSensor",
    security = 0,
    is_reachable = 1,
    is_battery_powered = false,

    endpoints = { endpoint_1, endpoint_2, endpoint_3 }
}

local events_handler = assert( loadfile( "HUB:zigbee/scripts/events/zigbee/events_handling" ) )

local zigbee = require( "zigbee" )

-- prepare zigbee
zigbee.add_node( node )

-- add node
events_handler( {
    event = "node_added",
    node_id = 42 
} )

local devices = core.get_devices()
for _, device in ipairs(devices) do
    tests.assert_eq(
        device.info, 
        {
            manufacturer = "EzloTest",
            model = "ComboSensorTest"
        }, 
        "Compare device info field"
    )
end
