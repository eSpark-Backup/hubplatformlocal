local tests = require( "tests" )
local core = require("core")
local zwave = require("zwave")
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local item_names = require( "HUB:zwave/scripts/resources/item_names" )
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

zwave.request_node_values = tests.create_mock()

core.send_device_firmware_update_broadcast = tests.create_mock()
zwave.start_rediscover = tests.create_mock()

local channel_0 = {
    id = 0,
    classes = { 
        { id = class_ids.battery },
        {
            id = class_ids.version,
            zwave_protocol_version = "1.0",
            hardware_version = 1,
            firmware_versions = { "1.0", "1.0" }
        },
        {
            id = class_ids.firmware_update,
            firmware_ids = { 1, 3 }
        }
    }
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

    channels = { channel_0 }
}

-- prepare zwave
zwave.add_node( node )

-- add node
events_handler( {
    event = "node_added",
    node_id = 9000
} )

local device_id = storage:call("find_device_id", 9000)

local function send_update_status(node_id, target, status, progress, error)
    events_handler( {
        event = "node_firmware_update_status",
        node_id = 9000,
        target = target,
        status = status,
        progress = progress,
        error = error
    } )
end


do
    send_update_status("9000", 0, "started")

    tests.assert_called(
        core.send_device_firmware_update_broadcast, 
        "check sending started broadcast",
        {
            {
                {
                    device_id = device_id,
                    firmware_id = "us.0.1.0",
                    status = "started"
                }
            }
        })

    tests.assert_eq(core.get_device(device_id).status, "updating", "check device status is updating")
end


do
    send_update_status("9000", 0, "in_progress")

    tests.assert_called(
        core.send_device_firmware_update_broadcast, 
        "check sending started broadcast",
        {
            {
                {
                    device_id = device_id,
                    firmware_id = "us.0.1.0",
                    status = "in_progress"
                }
            }
        })

    tests.assert_eq(core.get_device(device_id).status, "updating", "check device status is still 'updating'")
end

do
    send_update_status("9000", 0, "finished")

    tests.assert_called(
        core.send_device_firmware_update_broadcast, 
        "check sending started broadcast",
        {
            {
                {
                    device_id = device_id,
                    firmware_id = "us.0.1.0",
                    status = "finished"
                }
            }
        })

    tests.assert_eq(core.get_device(device_id).status, "updating", "check device status is still 'updating'")
end

do
    send_update_status("9000", 1, "started")

    tests.assert_called(
        core.send_device_firmware_update_broadcast, 
        "check sending started broadcast",
        {
            {
                {
                    device_id = device_id,
                    firmware_id = "us.0.3.1",
                    status = "started"
                }
            }
        })

    tests.assert_eq(core.get_device(device_id).status, "updating", "check device status is updating")
end

do
    send_update_status("9000", 1, "failed")

    tests.assert_called(
        core.send_device_firmware_update_broadcast, 
        "check sending started broadcast",
        {
            {
                {
                    device_id = device_id,
                    firmware_id = "us.0.3.1",
                    status = "failed"
                }
            }
        })

    tests.assert_eq(core.get_device(device_id).status, "updating", "check device status is updating")
    tests.assert_called(zwave.start_rediscover, "check start rediscover not called", { })
end

do
    send_update_status("9000", nil, "finished")

    tests.assert_called(
        core.send_device_firmware_update_broadcast, 
        "check send_device_firmware_update_broadcast not called",
        { } )

    tests.assert_eq(core.get_device(device_id).status, "rediscovering", "check device status became idle")
    tests.assert_called(zwave.start_rediscover, "check start rediscover called", { { 9000, tests.skipped_param } })
end
