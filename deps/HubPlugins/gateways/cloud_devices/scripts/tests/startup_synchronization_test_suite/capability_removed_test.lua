local cloud_devices = require("cloud_devices")
local utils = require("HUB:cloud_devices/scripts/tests/utils")

-- prepare

local cloud_device = {
    id = "cloud_device_id",
    battery_powered = 0,
    capabilities = {
        "text_command",
        "ui_flag_alexa_account"
    },
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

cloud_device.capabilities = {}

-- test

loadfile("HUB:cloud_devices/scripts/startup")()

-- check

utils.check_cloud_device_storage_associations(cloud_device.id)
utils.expect_devices({
    {
        id = utils.get_core_id("cloud_device_id"),
        battery_powered = false,
        category = "assistant",
        device_type_id = "alexa_account",
        gateway_id = "gateway",
        info = {
            abstract = "cloud_device_id",
            account = "amazoncorp@ezlo.com",
            vendor = "alexa"
        },
        name = "cloud_device_name",
        reachable = true,
        security = "high",
        subcategory = "voice",
        type = "assistant.voice",
        items = {}
    }
})
