local tests = require("tests")
local cloud_devices = require("cloud_devices")
local core = require("core")
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

cloud_devices.send_command = tests.create_mock()
core.send_response = tests.create_mock()

-- test

loadfile("HUB:cloud_devices/scripts/set_item_value")({
    item_id = utils.get_core_item_id(cloud_device.id, "text_command"),
    value = "What is your name?",
    operation_id = "set_item_value_operation_id"
})

-- check

tests.assert_called(cloud_devices.send_command,
        "command sent",
        { { "1", cloud_device.id, "text_command", "send", { text = "What is your name?" } } }
)

utils.expect_devices({
    {
        id = utils.get_core_id("cloud_device_id"),
        ready = false
    }
})

-- test

loadfile("HUB:cloud_devices/scripts/events/events_handling")({
    event = "command_response",
    command_id = "1",
    result = {},
    error = {}
})

-- check

tests.assert_called(core.send_response,
        "response sent",
        { { "set_item_value_operation_id", {}, {
            code = 3740,
            data = "custom_error",
            message = "Incorrect cloud command response result format"
        } } }
)

utils.expect_devices({
    {
        id = utils.get_core_id("cloud_device_id"),
        ready = true
    }
})
