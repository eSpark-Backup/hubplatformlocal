local zwave = require("zwave")
local core = require("core")
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local clear_item_dictionary = loadfile( "HUB:zwave/scripts/clear_item_dictionary" )
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local tests = require("tests")
local item_names = require("HUB:zwave/scripts/resources/item_names")

local timer = require "timer"

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local master_code_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/master_code_state")

zwave.request_node_values = tests.create_mock()

zwave.set_user_code = tests.create_mock()

local cc_user_code = {
    id = class_ids.user_codes,
    supported_users_count = 250,
    version = 2,
    capabilities = {
        checksum_supported = true,
        master_code_supported = true,
        master_code_deactivation_supported = false,
        supported_keys = "0123456789",
        supported_keypad_modes = { "normal", "vacation", "privacy", "locked_out" },
        supported_user_id_statuses = { "enabled", "disabled", "passage" }
    }
}

local node = {
    id = 9000,
    manufacturer_id = 0,
    product_type = 1,
    product_id = 2,
    is_battery_powered = true,

    specific_type = "",
    generic_type = "",

    security = { "none" },
    security_class_started = { "none" },

    channels = {  
        {
            id = 1,
            classes = { cc_user_code }
        }
    }
}

zwave.add_node( node )

-- add node
events_handler( {
    event = "node_added",
    node_id = 9000
} )

local user_codes_item_id = storage:call("find_item_id", item_names.user_codes, 9000, class_ids.user_codes, 1)

events_handler( {
    event = "user_code_updated",
    node_id = 9000,
    channel_id = 1,
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

events_handler( {
    event = "user_code_updated",
    node_id = 9000,
    channel_id = 1,
    class_id = class_ids.user_codes,
    user_polling_status = "idle",
    user_code_ranges = {
        {
            user_id = 10,
            user_status = "enabled",
            user_code = "1234"
        }
    }
} )


-- clear item dictionary


clear_item_dictionary({ item_id = user_codes_item_id })

tests.assert_called(zwave.set_user_code, "check set user code called", { { 9000, 1, 0, "available", "" } })

local user_code_requests_sender = hubplugins.object("sequent_requests_sender/user_codes", "HUB:zwave/scripts/utils", user_codes_item_id, 9000, 1)
tests.assert_eq( user_code_requests_sender:call("get_requests"), nil, "check no requests to sequent sending" )
tests.assert_eq( user_code_requests_sender:call("get_postponed_requests_set"), { ["8"] = true, ["10"] = true }, "check postponed requests" )

events_handler( {
    event = "user_code_updated",
    node_id = 9000,
    channel_id = 1,
    class_id = class_ids.user_codes,
    user_polling_status = "idle",
    user_code_ranges = {
        {
            user_id = 0,
            user_status = "available",
            user_code = ""
        }
    }
} )

tests.assert_eq( user_code_requests_sender:call("get_requests"), nil, "check no requests to sequent sending after" )
tests.assert_eq( user_code_requests_sender:call("get_postponed_requests_set"), nil, "check no postponed requests after report" )
