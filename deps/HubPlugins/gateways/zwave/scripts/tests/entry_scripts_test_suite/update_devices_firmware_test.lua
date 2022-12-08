local tests = require( "tests" )

local item_names = require("HUB:zwave/scripts/resources/item_names")
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local zwave = require( "zwave" )
local core = require( "core" )
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

local update_devices_firmware = loadfile("HUB:zwave/scripts/update_devices_firmware")

local timer = require "timer"

zwave.request_node_values = tests.create_mock()
zwave.update_nodes_firmware = tests.create_mock()

local function add_device(node_id, firmware_versions, firmware_ids)
    local node = {
        id = tostring(node_id),
        manufacturer_id = 1,
        product_type = 2,
        product_id = 3,
        is_battery_powered = true,

        specific_type = "",
        generic_type = "",

        security = { "none" },
        security_class_started = { "none" },


        channels = {
            {
                id = 0,
                classes = { 
                    { id = class_ids.switch_binary },
                    {
                        id = class_ids.version,
                        zwave_protocol_version = "1.0",
                        hardware_version = 1,
                        firmware_versions = firmware_versions
                    },
                    {
                        id = class_ids.firmware_update,
                        firmware_ids = firmware_ids
                    }
                }
            }
        }
    }

    zwave.add_node( node )

    events_handler( {
        event = "node_added",
        node_id = node_id
    } )

    return storage:call("find_device_id", node_id)
end 

local device_id_1 = add_device(
        101,
        { "1.0", "1.0" },
        { 2, 2 }
    )

local device_id_2 = add_device(
        102,
        { "1.0", "1.0" },
        { 2, 2 }
    )

local device_id_3 = add_device(
        103,
        { "1.0" },
        { 2 }
    )

do
    update_devices_firmware( {
        devices = { 
            device_id_1, 
            device_id_2 
        }, 
        firmware = {
            {
                id = "us.1.2.0",
                image = "image0",
                checksum = "checksum0"
            },
            {
                id = "us.1.2.1",
                image = "image1",
                checksum = "checksum1"
            }
        } 
    })

    tests.assert_called(
        zwave.update_nodes_firmware, 
        "check update_nodes_firmware successfully called", 
        {
            { 
                { 
                    "101", 
                    "102" 
                },  
                {
                    {
                        target = 0,
                        image = "image0",
                        checksum = "checksum0"
                    },
                    {
                        target = 1,
                        image = "image1",
                        checksum = "checksum1"
                    }
                }
            }
        })
end

do
    tests.assert_failed( 
        update_devices_firmware, 
        {
            devices = { 
                device_id_1, 
                device_id_2 
            }, 
            firmware = {
                {
                    id = "us.1.2.0",
                    image = "image0",
                    checksum = "checksum0"
                },
                {
                    id = "us.1.2.2",
                    image = "image1",
                    checksum = "checksum1"
                }
            } 
        })
end

do
    tests.assert_failed( 
        update_devices_firmware, 
        {
            devices = { 
                device_id_1, 
                device_id_3 
            }, 
            firmware = {
                {
                    id = "us.1.2.0",
                    image = "image0",
                    checksum = "checksum0"
                },
                {
                    id = "us.1.2.1",
                    image = "image1",
                    checksum = "checksum1"
                }
            } 
        })

    tests.assert_failed( 
        update_devices_firmware, 
        {
            devices = { 
                device_id_3, 
                device_id_2 
            }, 
            firmware = {
                {
                    id = "us.1.2.1",
                    image = "image1",
                    checksum = "checksum1"
                },
                {
                    id = "us.1.2.0",
                    image = "image0",
                    checksum = "checksum0"
                }
            } 
        })

    tests.assert_failed( 
        update_devices_firmware, 
        {
            devices = { 
                device_id_3
            }, 
            firmware = {
                {
                    id = "us.1.2.0",
                    image = "image0",
                    checksum = "checksum0"
                },
                {
                    id = "us.1.2.1",
                    image = "image1",
                    checksum = "checksum1"
                }
            } 
        })
end