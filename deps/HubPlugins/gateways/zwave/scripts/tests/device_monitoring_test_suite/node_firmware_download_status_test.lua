local tests = require( "tests" )
local core = require("core")
local zwave = require("zwave")
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local item_names = require( "HUB:zwave/scripts/resources/item_names" )

zwave.request_node_values = tests.create_mock()

core.send_device_firmware_download_broadcast = tests.create_mock()

local channel_0 = {
    id = 0,
    classes = { { id = class_ids.battery } }
}

local node = {
    id = "9000",
    manufacturer_id = 0,
    product_type = 1,
    product_id = 2,
    is_battery_powered = true,

    specific_type = "",
    generic_type = "",

    security = { "none" },
    security_class_started = { "none" },

    channels = { channel_0 },

    version_info = {
        hardware_version = 1,
        zwave_protocol_version = "1.0",
        firmware =  {
            {
                id = "0.1.0",
                version = "1.0"
            },
            {
                id = "0.1.1",
                version = "1.0"
            }
        }
    },
}

-- prepare zwave
zwave.add_node( node )

-- add node
events_handler( {
    event = "node_added",
    node_id = 9000
} )

local function send_download_status(image, checksum, status, progress, error)
    events_handler( {
        event = "node_firmware_download_status",
        image = image,
        checksum = checksum,
        node_id = 9000,
        status = status,
        progress = progress,
        error = error
    } )
end

do
    send_download_status("some_image", "some_checksum", "started")

    tests.assert_called(
        core.send_device_firmware_download_broadcast, 
        "check sending started broadcast",
        {
            {
                {
                    image = "some_image",
                    checksum = "some_checksum",
                    status = "started"
                }
            }
        })
end

do
    send_download_status("some_image", "some_checksum", "in_progress", 11)

    tests.assert_called(
        core.send_device_firmware_download_broadcast, 
        "check sending started broadcast",
        {
            {
                {
                    image = "some_image",
                    checksum = "some_checksum",
                    status = "in_progress",
                    progress = 11
                }
            }
        })
end

do
    send_download_status("some_image", "some_checksum", "failed", 0, "error message")

    tests.assert_called(
        core.send_device_firmware_download_broadcast, 
        "check sending started broadcast",
        {
            {
                {
                    image = "some_image",
                    checksum = "some_checksum",
                    status = "failed",
                    progress = 0,
                    error = "error message"
                }
            }
        })
end
