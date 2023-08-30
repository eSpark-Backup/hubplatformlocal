local twogig = require("2gig")
local core = require("core")
local tests = require("tests")
local add_device = loadfile("HUB:2gig/scripts/add_device")
local remove_device = loadfile("HUB:2gig/scripts/remove_device")
local storage_utils = require("HUB:2gig/scripts/helpers/storage_utils")

add_device({
    model = "2GIG-FF-345",
    serial = 123
})

tests.assert_eq(#core.get_devices(), 2, "initial devices count")
tests.assert_eq(#core.get_items(), 6, "initial items count")
tests.assert_eq(#twogig.get_devices(), 2, "initial TXIDs registered count")

remove_device({
    deviceId = storage_utils.find_device_id(124)
})

tests.assert_eq(#(core.get_devices() or {}), 0, "devices count after removing child")
tests.assert_eq(#core.get_items(), 0, "items count after removing child")
tests.assert_eq(#twogig.get_devices(), 0, "TXIDs registered count after removing child")
