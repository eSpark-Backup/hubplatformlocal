local twogig = require("2gig")
local core = require("core")
local tests = require("tests")
local set_setting_value = loadfile("HUB:2gig/scripts/set_setting_value")
local setting_factory = require("HUB:2gig/scripts/helpers/setting_factory")
local storage_utils = require("HUB:2gig/scripts/helpers/storage_utils")

twogig.configure_device = tests.create_mock()

local tx_id = 123
local core_device_id = "device_id_1"
storage_utils.register_device(tx_id, "device_id_1")

local setting = setting_factory.create_number_setting({
    tag = "hello",
    label = "Label",
    description = "Description",
    default = 20,
    min = 10,
    max = 30
})
setting.device_id = core_device_id

local core_setting_id = core.add_setting(setting)

set_setting_value({
    setting_id = core_setting_id,
    value = 15
})

tests.assert_eq(core.get_setting(core_setting_id).value, 15, "check core setting value")
tests.assert_eq(core.get_setting(core_setting_id).status, "synced", "check core setting status")
tests.assert_called(twogig.configure_device, "Check twogig.configure_device call", {
    { { tx_id }, { hello = 15 } }
})

set_setting_value({
    setting_id = core_setting_id,
    value = 5
})

tests.assert_eq(core.get_setting(core_setting_id).value, 10, "check core setting value")
tests.assert_eq(core.get_setting(core_setting_id).status, "synced", "check core setting status")
tests.assert_called(twogig.configure_device, "Check twogig.configure_device call", {
    { { tx_id }, { hello = 10 } }
})

set_setting_value({
    setting_id = core_setting_id,
    value = 33
})

tests.assert_eq(core.get_setting(core_setting_id).value, 30, "check core setting value")
tests.assert_eq(core.get_setting(core_setting_id).status, "synced", "check core setting status")
tests.assert_called(twogig.configure_device, "Check twogig.configure_device call", {
    { { tx_id }, { hello = 30 } }
})

tests.assert_eq( pcall(set_setting_value, { setting_id = core_setting_id, value = "111" }), false, "check wrong type" )
tests.assert_eq(core.get_setting(core_setting_id).value, 30, "check core setting value")
