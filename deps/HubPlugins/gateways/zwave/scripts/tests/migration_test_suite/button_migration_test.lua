local zwave = require("zwave")
local storage = require("storage")
local tests = require("tests")
local events_handler = loadfile("HUB:zwave/scripts/events/events_handling")
local class_ids = require("HUB:zwave/scripts/resources/class_ids")

zwave.request_node_values = tests.create_mock()

local run_migration = loadfile("HUB:zwave/scripts/tests/migration_test_suite/run_migration")
local check_devices = loadfile("HUB:zwave/scripts/tests/device_adding_removing_test_suite/check_zwave_node_adding_result")

local default_changes = require("HUB:zwave/scripts/migration/version_changes/default")
default_changes.check_all_nodes = true
loadfile = loadfile("HUB:zwave/scripts/tests/migration_test_suite/setup_versions")(storage, default_changes, 1, 2)

zwave.add_node({
    id = "9000",

    manufacturer_id = 1,
    product_type = 2,
    product_id = 3,

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
                    id = class_ids.central_scene,
                    buttons = {
                        { button_id = "1", button_attributes = { "press_1_time" } },
                        { button_id = "2", button_attributes = { "press_1_time" } }
                    }
                }
            }
        }
    }
})

events_handler({
    event = "node_added",
    node_id = 9000
})

run_migration()

local function make_button_state_item(button_number, states)
    return {
        name = "button_state",
        value_type = "button_state",
        enum = states,
        tag = button_number,
        value = {
            button_number = button_number,
            button_state = "idle"
        },
        cc_id = class_ids.central_scene,
        channel_id = 0
    }
end

check_devices({
    {
        node_id = "9000",
        name = "Scene Controller",
        category = "scene_controller",
        subcategory = "generic", 
        security = "no",
        battery_powered = false,
        status = "idle",

        items = {
            make_button_state_item("1", { "press_1_time", "idle" }),
            make_button_state_item("2", { "press_1_time", "idle" })
        },

        subdevices = {}
    }
})
