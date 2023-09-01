local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/base_fixture")()
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local zwave = require("zwave")
local core = require("core")
local tests = require("tests")

zwave.request_node_values = tests.create_mock()

local cc_switch = {
    id = class_ids.switch_binary,
}

local channel_0 = {
    id = 0,
    classes = { cc_switch }
}

local channel_1 = {
    id = 1,
    classes = { cc_switch }
}

local node = {
    id = "9000",
    manufacturer_id = 0,
    product_type = 0,
    product_id = 0,
    is_battery_powered = false,

    specific_type = "SPECIFIC_TYPE_POWER_SWITCH_BINARY",
    generic_type = "GENERIC_TYPE_SWITCH_BINARY",

    security = { "none" },
    security_class_started = { "none" },

    channels = { channel_0, channel_1 }
}

fixture:add_node( node )

tests.assert_eq(#core.get_devices(), 2)
tests.assert_eq(#core.get_items(), 2)

-- Note: simulate a device without a corresponding node ID
storage:call("remove_device_mapping", 9000)
loadfile("HUB:zwave/scripts/start_exclude")({
    deviceId = "device_id_2"
})

tests.assert_eq(core.get_devices(), nil)
tests.assert_eq(next(core.get_items()), nil)
