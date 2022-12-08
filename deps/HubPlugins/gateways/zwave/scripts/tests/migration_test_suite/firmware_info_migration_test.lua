local zwave = require("zwave")
local core = require("core")
local storage = require("storage")
local tests = require("tests")
local events_handler = loadfile("HUB:zwave/scripts/events/events_handling")

zwave.request_node_values = tests.create_mock()

local run_migration = loadfile("HUB:zwave/scripts/tests/migration_test_suite/run_migration")
local check_devices = loadfile("HUB:zwave/scripts/tests/device_adding_removing_test_suite/check_zwave_node_adding_result")

local default_changes = require("HUB:zwave/scripts/migration/version_changes/default")
default_changes.check_all_nodes = true
loadfile = loadfile("HUB:zwave/scripts/tests/migration_test_suite/setup_versions")(storage, default_changes, 1, 2)

local class_ids = require("HUB:zwave/scripts/resources/class_ids")

zwave.add_node({
    id = "9000",

    manufacturer_id = 10,
    product_type = 11,
    product_id = 12,

    generic_type = "GENERIC_TYPE_SWITCH_BINARY",
    specific_type = "",

    security = { "none" },
    security_class_started = { "none" },

    is_battery_powered = false,

    channels = {
        {
            id = 0,
            classes = {
                {
                    id = class_ids.version,
                    zwave_protocol_version = "1.0",
                    hardware_version = 1,
                    firmware_versions = { "10.1" }
                },
                {
                    id = class_ids.firmware_update,
                    firmware_ids = { 1 }
                }
            }
        }
    }
})

events_handler({
    event = "node_added",
    node_id = 9000
})

tests.assert_eq(core.get_devices()[ 1 ].info, {
    manufacturer = "manufacturer_10",
    model = "Unknown",
    ["firmware.stack"] = "1.0",
    ["zwave.smartstart"] = "no",
    ["zwave.node"] = "9000",
    ["channels.count"] = "1",
    hardware = "1",
    protocol = "zwave"
})

tests.assert_eq(core.get_devices()[ 1 ].firmware, {
    {
        id = "us.10.1.0",
        version = "10.1"
    }
})

zwave.add_node({
    id = "9000",

    manufacturer_id = 10,
    product_type = 11,
    product_id = 12,

    generic_type = "GENERIC_TYPE_SWITCH_BINARY",
    specific_type = "",

    security = { "none" },
    security_class_started = { "none" },

    is_battery_powered = false,

    channels = {
        {
            id = 0,
            classes = {
                {
                    id = class_ids.version,
                    zwave_protocol_version = "2.0",
                    hardware_version = 2,
                    firmware_versions = { "20.1", "21.1" }
                },
                {
                    id = class_ids.firmware_update,
                    firmware_ids = { 2, 2 }
                }
            }
        }
    }
})

run_migration()

tests.assert_eq(core.get_devices()[ 1 ].status, "idle")

tests.assert_eq(core.get_devices()[ 1 ].info, {
    manufacturer = "manufacturer_10",
    model = "Unknown",
    ["firmware.stack"] = "2.0",
    ["zwave.smartstart"] = "no",
    ["zwave.node"] = "9000",
    hardware = "2",
    protocol = "zwave"
})

tests.assert_eq(core.get_devices()[ 1 ].firmware, {
    {
        id = "us.10.2.0",
        version = "20.1"
    },
    {
        id = "us.10.2.1",
        version = "21.1"
    }
})
