local cloud_devices = require("cloud_devices")
local utils = require("HUB:cloud_devices/scripts/tests/utils")

-- prepare

local cloud_device = {
    id = "cloud_device_id",
    battery_powered = 0,
    capabilities = {},
    category = "assistant",
    device_type_id = "alexa_account",
    info = {
        account = "amazoncorp@ezlo.com",
        vendor = "alexa"
    },
    name = "cloud_device_name",
    protocol = {
        vendor = "alexa"
    },
    reachable = 1,
    reachable_reason = "assistant.voice",
    security = "high",
    subcategory = "voice",
    type = "assistant.voice"
}

cloud_devices.add_device(cloud_device)

loadfile("HUB:cloud_devices/scripts/startup")()

cloud_devices.remove_device(cloud_device.id)

-- test

loadfile("HUB:cloud_devices/scripts/startup")()

-- check

utils.check_no_storage_associations()
utils.expect_devices({})
