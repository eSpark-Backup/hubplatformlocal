local core = require("core")
local tests = require("tests")
local add_device = loadfile("HUB:2gig/scripts/add_device")
local remove_device = loadfile("HUB:2gig/scripts/remove_device")
local storage_utils = require("HUB:2gig/scripts/helpers/storage_utils")

add_device({
    model = "2GIG-FF-345",
    serial = 123
})

add_device({
    model = "2GIG-FF-345",
    serial = 456
})

tests.assert_eq(#core.get_devices(), 4, "initial devices count")

remove_device({
    deviceId = storage_utils.find_device_id(123)
})

tests.assert_eq(#core.get_devices(), 2, "devices count after removing one")
