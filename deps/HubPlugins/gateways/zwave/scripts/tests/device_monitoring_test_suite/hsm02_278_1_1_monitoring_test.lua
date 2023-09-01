local item_names = require("HUB:zwave/scripts/resources/item_names")
local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local item_tags = require("HUB:zwave/scripts/resources/item_descriptor_tags")
local events_handler = hubplugins.weak_loadfile( "HUB:zwave/scripts/events/events_handling" )
local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/hsm02_278_1_1_fixture")()
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local zwave = require("zwave")
local core = require("core")
local tests = require( "tests" )

zwave.request_node_values = tests.create_mock()

core.update_item_value = tests.create_mock()

local node = fixture:create_node("9000")

fixture:add_node(node)

local motion_item_id = storage:call("find_item_id", item_names.motion, 9000, class_ids.sensor_binary, 0, item_tags.home_security)
local security_threat_item_id = storage:call("find_item_id", item_names.motion, 9000, class_ids.sensor_binary, 0, "home_security.security_tag")
local battery_item_id = storage:call("find_item_id", item_names.battery, 9000, class_ids.battery, 0)

do -- send binary sensor
    events_handler( {
        event = "binary_sensor_value_updated",
        node_id = "9000",
        channel_id = 0,
        class_id = class_ids.sensor_binary,
        sensor_type = 0x00,
        value = 0xFF,
    } )

    tests.assert_called(
        core.update_item_value,
        "check if value updated on unknown sensor binary type",
        {
            { motion_item_id, true },
            { security_threat_item_id, true },
        }
    )
end


do -- send battery update
    events_handler( {
        event = "value_updated",
        node_id = 9000,
        class_id = class_ids.battery,
        channel_id = 0,
        value = 50
    } )

    tests.assert_called(
            core.update_item_value,
            "check if battery value updated",
            { { battery_item_id, 50 }, }
    )
end
