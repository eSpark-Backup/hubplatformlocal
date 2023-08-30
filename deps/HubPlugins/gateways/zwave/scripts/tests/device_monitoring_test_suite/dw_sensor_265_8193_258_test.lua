local tests = require( "tests" )
local zwave = require("zwave")
local core = require("core")
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local item_names = require("HUB:zwave/scripts/resources/item_names")
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local item_descriptor_tags = require("HUB:zwave/scripts/resources/item_descriptor_tags")
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

local fixture = loadfile("HUB:zwave/scripts/tests/fixtures/dw_sensor_265_8193_258_fixture")()

zwave.request_node_values = tests.create_mock()

local node = fixture:create_node()

fixture:add_node(node)

local tampering_cover_alarm_item_id = storage:call(
    "find_item_id", 
    item_names.tampering_cover_alarm, 
    9000, 
    class_ids.notification, 
    0, 
    item_descriptor_tags.home_security
)
assert(tampering_cover_alarm_item_id)

core.update_item_value = tests.create_mock()

local function send_product_cover_removed_event()
    events_handler( {
    event = "notification_recieved",
    node_id = 9000,
    class_id = class_ids.notification,
    channel_id = 0,
    v1_alarm_type = 0x07,
    v1_alarm_level = 0xFF,
    event_type = "product_cover_removed",
    value = 0xFF,
    notification_type = "home_security"
} )
end


do
    send_product_cover_removed_event()

    tests.assert_called(core.update_item_value, "", {{ tampering_cover_alarm_item_id, "tampering_cover" }})
end


do
    events_handler( {
        event = "notification_recieved",
        node_id = 9000,
        class_id = class_ids.notification,
        channel_id = 0,
        v1_alarm_type = 0x07,
        v1_alarm_level = 0xFF,
        event_type = "intrusion",
        value = 0xFF,
        notification_type = "home_security"
    } )

    tests.assert_called(core.update_item_value, "", {{ tampering_cover_alarm_item_id, "no_tampering_cover" }})
end


do
    send_product_cover_removed_event()

    tests.assert_called(core.update_item_value, "", {{ tampering_cover_alarm_item_id, "tampering_cover" }})
end



do
    events_handler( {
        event = "notification_recieved",
        node_id = 9000,
        class_id = class_ids.notification,
        channel_id = 0,
        v1_alarm_type = 0x07,
        v1_alarm_level = 0,
        event_type = "state_idle",
        notification_type = "home_security"
    } )

    tests.assert_called(core.update_item_value, "", {{ tampering_cover_alarm_item_id, "no_tampering_cover" }})
end