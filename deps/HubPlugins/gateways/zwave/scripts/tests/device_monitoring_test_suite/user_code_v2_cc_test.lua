local zwave = require("zwave")
local core = require("core")
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local item_value_type = require("HUB:zwave/scripts/resources/item_value_type")
local tests = require("tests")
local item_names = require("HUB:zwave/scripts/resources/item_names")

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

zwave.request_node_values = tests.create_mock()

local cc_user_code = {
    id = class_ids.user_codes,
    supported_users_count = 250,
    version = 2,
    capabilities = {
        checksum_supported = true,
        master_code_supported = true,
        master_code_deactivation_supported = false,
        supported_keys = "0123456789",
        supported_keypad_modes = { "normal", "vacation", "privacy" },
        supported_user_id_statuses = { "enabled", "disabled", "passage" }
    }
}

local node = {
    id = "9000",
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

local function find_item_id(item_name)
    return storage:call("find_item_id", 
                            item_name, 
                            "9000", 
                            class_ids.user_codes, 
                            1 )
end

-- update values

events_handler( {
    event = "user_code_updated",
    node_id = "9000",
    class_id = class_ids.user_codes,
    channel_id = 1,
    user_polling_status = "idle",
    user_code_ranges = {
        {
            user_id = 7,
            user_status = "enabled",
            user_code = "1111"
        }
    },
    supported_users_count = 250,
    user_code = "1111"
} )

events_handler( {
    event = "user_code_updated",
    node_id = "9000",
    class_id = class_ids.user_codes,
    channel_id = 1,
    user_code_ranges = {
        {
            user_id = 9,
            user_status = "privacy",
            user_code = "2222"
        },
        {
            user_id = 11,
            user_status = "passage",
            user_code = "3333"
        }
    },
    user_polling_status = "idle",
    supported_users_count = 250
} )

events_handler( {
    event = "master_code_updated",
    node_id = "9000",
    channel_id = 1,
    class_id = class_ids.user_codes,
    value = "1234"
} )

events_handler( {
    event = "user_codes_keypad_mode_updated",
    node_id = "9000",
    channel_id = 1,
    class_id = class_ids.user_codes,
    value = "privacy"
} )


-- update checksum (actualy handler does nothing)
events_handler( {
    event = "user_codes_checksum_updated",
    node_id = "9000",
    channel_id = 1,
    class_id = class_ids.user_codes,
    value = 8033
} )

-- check core values

local master_code_item_id = find_item_id(item_names.master_code)
local user_codes_keypad_mode_item_id = find_item_id(item_names.user_codes_keypad_mode)
local user_codes_item_id = find_item_id(item_names.user_codes)

tests.assert_eq( core.get_item( master_code_item_id ).value, "1234", "Check master code" )
tests.assert_eq( core.get_item( user_codes_keypad_mode_item_id ).value, "privacy", "Check keypad mode" )

tests.assert_eq( 
    core.get_item( user_codes_item_id ).value, 
    {
        ["7"] = {
            name = tests.skipped_param,
            code = "1111",
            mode = "enabled"
        },
        ["9"] = {
            name = tests.skipped_param,
            code = "2222",
            mode = "privacy"
        },
        ["11"] = {
            name = tests.skipped_param,
            code = "3333",
            mode = "passage"
        },
    },
    "Check user codes" )

do
    events_handler( {
        event = "user_code_updated",
        node_id = "9000",
        class_id = class_ids.user_codes,
        channel_id = 1,
        user_code_ranges = {
            {
                user_id = 9,
                last_user_id = 11,
                user_status = "available",
                user_code = ""
            }
        },
        user_polling_status = "idle",
        supported_users_count = 250
    } )

    tests.assert_eq( 
    core.get_item( user_codes_item_id ).value, 
    {
        ["7"] = {
            name = tests.skipped_param,
            code = "1111",
            mode = "enabled"
        }
    },
    "Check user codes are removed after available status receiving" )
end

-- remove node
events_handler( {
    event = "node_removed",
    node_id = "9000",
    manufacturer_id = 0,
    product_type = 1,
    product_id = 2,
} )

-- Check storage is cleared

tests.assert_eq( 
    storage:call("get_string", storage:call("get_master_code_key_name", master_code_item_id)), 
    nil, 
    "Check master code cache is cleared" )

tests.assert_eq( 
    storage:call("get_table", storage:call("get_usernames_tablename", user_codes_item_id)), 
    nil, 
    "Check user codes names cache is cleared" )
