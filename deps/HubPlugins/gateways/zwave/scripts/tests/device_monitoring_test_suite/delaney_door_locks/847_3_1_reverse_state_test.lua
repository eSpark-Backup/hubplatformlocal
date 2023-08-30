local tests = require("tests")
local zwave = require("zwave")
local core = require("core")
local timer = require("timer")
local events_handler = assert(loadfile("HUB:zwave/scripts/events/events_handling"))
local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local item_names = require("HUB:zwave/scripts/resources/item_names")
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")


zwave.request_node_values = tests.create_mock()
zwave.set_door_lock_mode = tests.create_mock()

local channel_0 = {
    id = 0,
    classes = {
        { id = class_ids.user_codes, supported_users_count = 1 },
        { id = class_ids.configuration, version = 1 },
        { id = class_ids.door_lock }
    }
}

local node = {
    id = "9000",
    manufacturer_id = 847,
    product_type = 3,
    product_id = 1,
    is_battery_powered = true,

    specific_type = "SPECIFIC_TYPE_DOOR_LOCK",
    generic_type = "GENERIC_TYPE_ENTRY_CONTROL",

    security = { "none" },
    security_class_started = { "none" },

    channels = { channel_0 }
}

-- prepare zwave
zwave.add_node( node )

-- add node
events_handler( {
    event = "node_added",
    node_id = 9000
} )


local function send_door_lock_state(mode, condition)
    events_handler( {
        event = "door_lock_state_updated",
        node_id = 9000,
        class_id = class_ids.door_lock,
        channel_id = 0,
        door_lock_mode = mode,
        door_condition = condition
    } )
end

local door_lock_item_id = storage:call("find_item_id", item_names.door_lock, 9000, class_ids.door_lock, 0)


send_door_lock_state("secured")
tests.assert_eq(core.get_item(door_lock_item_id).value, "unsecured", "Check secured mode sent is unsecured")

send_door_lock_state("unsecured")
tests.assert_eq(core.get_item(door_lock_item_id).value, "secured", "Check unsecured mode sent is secured")


local set_item_value = loadfile( "HUB:zwave/scripts/set_item_value" )

local request_door_lock_state_timer = hubplugins.object("request_value_timer", 
                                                        "HUB:zwave/scripts/timer_handlers", 
                                                        9000, 
                                                        0, 
                                                        class_ids.door_lock, 
                                                        "request_door_lock_state")

set_item_value({ item_id = door_lock_item_id, value = "secured" })
tests.assert_called(zwave.set_door_lock_mode, "check set unsecured mode after set secured value", { { "9000", 0, "unsecured" } })
tests.assert_eq(timer.get_time_to_execution(request_door_lock_state_timer._timer_id), 4000, "check time to execution")
request_door_lock_state_timer:call("stop")


set_item_value({ item_id = door_lock_item_id, value = "unsecured" })
tests.assert_called(zwave.set_door_lock_mode, "check set secured mode after set unsecured value", { { "9000", 0, "secured" } })
tests.assert_eq(timer.get_time_to_execution(request_door_lock_state_timer._timer_id), 4000, "check time to execution")
request_door_lock_state_timer:call("stop")
