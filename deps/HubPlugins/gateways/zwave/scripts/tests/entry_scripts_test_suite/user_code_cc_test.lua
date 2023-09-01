local zwave = require("zwave")
local core = require("core")
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local set_item_value = loadfile( "HUB:zwave/scripts/set_item_value" )
local set_item_dictionary_value = loadfile( "HUB:zwave/scripts/set_item_dictionary_value" )
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local tests = require("tests")
local item_names = require("HUB:zwave/scripts/resources/item_names")

local timer = require "timer"

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local master_code_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/master_code_state")

zwave.request_node_values = tests.create_mock()

zwave.set_master_code = tests.create_mock()
zwave.request_master_code = tests.create_mock()
zwave.set_user_code = tests.create_mock()
zwave.request_user_code = tests.create_mock()
zwave.set_user_codes_keypad_mode = tests.create_mock()
zwave.request_user_codes_keypad_mode = tests.create_mock()

local node_id = 9000
local channel_0_id = 0

local channel_0 = {
    id = channel_0_id,
    classes = {
        { id = class_ids.user_codes, supported_users_count = 250 },
    }
}

local node = {
    id = tostring(node_id),
    manufacturer_id = 0,
    product_type = 1,
    product_id = 2,
    is_battery_powered = true,

    specific_type = "",
    generic_type = "",

    security = { "none" },
    security_class_started = { "none" },

    channels = { channel_0 }
}

zwave.add_node( node )

-- add node
events_handler( {
    event = "node_added",
    node_id = "9000"
} )

local function find_item_id(item_name)
    return storage:call("find_item_id", 
                        item_name, 
                        "9000", 
                        class_ids.user_codes,
                        0)
end

local function get_item_value(item_name)
    return core.get_item(find_item_id(item_name)).value
end

local user_code_requests_sender = hubplugins.object("sequent_requests_sender/user_codes", "HUB:zwave/scripts/utils", find_item_id(item_names.user_codes), 9000, 0)

-- set user code without mode
do
    set_item_dictionary_value({
            item_id = find_item_id(item_names.user_codes),
            key = "8",
            value = {
                name = "xyz",
                code = "5567"
            },
            value_type = "userCode"
        })

    tests.assert_called(zwave.set_user_code, "Check set_user_code called", { { "9000", 0, 8, "enabled", "5567" } })
    tests.assert_called(zwave.request_user_code, "Check request_user_code not called immediatelly", { })

    tests.assert_eq( { user_code_requests_sender:call("get_requests") }, { }, "check no requests in requests sender" )
    tests.assert_eq( user_code_requests_sender:call("get_postponed_requests_set"), { ["8"] = true }, "check postponed requests" )

    events_handler( {
        event = "user_code_updated",
        node_id = "9000",
        channel_id = 0,
        class_id = class_ids.user_codes,
        user_polling_status = "idle",
        user_code_ranges = {
            {
                user_id = 8,
                user_status = "enabled",
                user_code = "5567"
            }
        }
    } )

    tests.assert_eq( user_code_requests_sender:call("get_postponed_requests_set"), nil, "check no postponed requests" )
end

-- set user code with mode
do
    set_item_dictionary_value({
            item_id = find_item_id(item_names.user_codes),
            key = "9",
            value = {
                name = "xyz",
                code = "5567",
                mode = "disabled"
            },
            value_type = "userCode"
        })

    tests.assert_called(zwave.set_user_code, "Check set_user_code called", { { "9000", 0, 9, "disabled", "5567" } })
    tests.assert_called(zwave.request_user_code, "Check request_user_code not called immediatelly", { })

    tests.assert_eq( { user_code_requests_sender:call("get_requests") }, { }, "check no requests in requests sender" )
    tests.assert_eq( user_code_requests_sender:call("get_postponed_requests_set"), { ["9"] = true }, "check postponed requests" )

    events_handler( {
        event = "user_code_updated",
        node_id = "9000",
        channel_id = 0,
        class_id = class_ids.user_codes,
        user_polling_status = "idle",
        user_code_ranges = {
            {
                user_id = 9,
                user_status = "disabled",
                user_code = "5567"
            }
        }
    } )
end

-- check nothing called more
tests.assert_called(zwave.set_user_code, "Check set_user_code was not called more", { })
tests.assert_called(zwave.request_user_code, "Check request_user_code was not called more", { })
tests.assert_called(zwave.set_user_codes_keypad_mode, "Check set_user_codes_keypad_mode was not called more", { })
tests.assert_called(zwave.request_user_codes_keypad_mode, "Check request_user_codes_keypad_mode was not called more", { })
tests.assert_called(zwave.set_master_code, "Check set_master_code was not called more", { })
tests.assert_called(zwave.request_master_code, "Check request_master_code was not called more", { })

tests.assert_eq( user_code_requests_sender:call("get_postponed_requests_set"), nil, "check no postponed requests" )
tests.assert_eq( { user_code_requests_sender:call("get_requests") }, { }, "check no requests in requests sender" )
