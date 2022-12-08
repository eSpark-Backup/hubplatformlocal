local zwave = require("zwave")
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local tests = require "tests"
local core = require( "core" )

tests.override_function(_G, "loadfile", function(orig_loadfile, path)
    if path == "HUB:zwave/scripts/model/devices/devices_info/generated/999_0_0" then
        return function()
            local info = orig_loadfile("HUB:zwave/scripts/model/devices/devices_info/base_info")()
            info.info = {manufacturer = "EzloTest", model = "ComboSensorTest"}
            return info
        end
    else
        return orig_loadfile(path)
    end
end)

zwave.request_node_values = tests.create_mock()

local cc_switch = { 
    id = class_ids.switch_binary,
}

local cc_sensor_multilevel = {
    id = class_ids.multilevel_sensor,
    sensors = {
        {scales = "watt", sensor_type = "power"}
    }
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

local channel_0 = { 
    id = 0,
    classes = { 
        cc_switch, 
        cc_sensor_multilevel,
        cc_version,
        cc_firmware_update
    }
}

local channel_1 = { 
    id = 1,
    classes = { cc_switch }
}


local channel_2 = { 
    id = 2,
    classes = { cc_switch }
}

local node = {
    id = "9000",
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

zwave.add_node( node )

zwave.get_manufacturer_name = function(manufacturer_id)
    if manufacturer_id == 999 then
        return "Ezlo"
    else
        return "Unknown"
    end
end

-- add node
events_handler( {
    event = "node_added",
    node_id = 9000 
} )

local devices = core.get_devices()
for _, device in ipairs(devices) do
    local expected_info = {
        manufacturer = "EzloTest",
        model = "ComboSensorTest",
        protocol = "zwave",
        ["zwave.smartstart"] = "no",
        ["zwave.node"] = "9000"
    }

    local expected_firmware

    if device.parent_device_id == "" then
        expected_info["hardware"] = "1"
        expected_info["firmware.stack"] = "1.0"
        expected_info["channels.count"] = "3"

        expected_firmware = {
            { id = "us.999.1.0", version = "1.0" }
        }
    end

    tests.assert_eq(
        device.info,
        expected_info,
        "Compare device info field"
    )

    tests.assert_eq(
        device.firmware,
        expected_firmware,
        "Compare device firmware field"
    ) 
end
