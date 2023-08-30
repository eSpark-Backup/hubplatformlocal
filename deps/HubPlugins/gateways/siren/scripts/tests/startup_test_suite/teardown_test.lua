local tests=require "tests"
local core = require("core")

core.subscribe = tests.create_mock()

local siren_device = loadfile("HUB:siren/scripts/device/siren")()
local add_device_res = core.add_device(siren_device)
core.add_device = tests.create_mock(add_device_res)

-- test
assert(loadfile("HUB:siren/scripts/startup")())
assert(loadfile("HUB:siren/scripts/teardown")())

--tests.assert_called(core.add_device, "add_device called", function() end, 1)
-- tests.assert_called(core.add_device, "add_device called", add_device_res, 1)

-- tests.assert_eq( #(core.get_devices() or {}), 1)
-- tests.assert_eq( #(core.get_items() or {}), 4)

-- check