local zwave = require("zwave")
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local item_value_type = require("HUB:zwave/scripts/resources/item_value_type")
local tests = require("tests")
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local core = require("core")

zwave.request_node_values = tests.create_mock()

zwave.set_user_code = tests.create_mock()


local cc_user_code = {
    id = class_ids.user_codes,
    supported_users_count = 250
}

local channel_0 = {
    id = 0,
    classes = { cc_user_code }
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

    channels = { channel_0 }
}

zwave.add_node( node )

-- add node
events_handler( {
    event = "node_added",
    node_id = 9000
} )

local user_codes_item_id = storage:call("find_item_id", 
    "user_codes", 
    "9000", 
    class_ids.user_codes, 
    channel_0.id )

tests.assert_ne(user_codes_item_id, nil, "check item id")

local external_setter = loadfile( "HUB:zwave/scripts/set_item_dictionary_value" )

local function send_user_code(user_status_id, user_id, code)
    events_handler( {
        event = "user_code_updated",
        node_id = 9000,
        class_id = class_ids.user_codes,
        channel_id = 0,
        
        user_code_ranges = {
            {
                user_id = user_id,
                user_status = user_status_id,
                user_code = code
            }
        },
        user_polling_status = "idle",
        supported_users_count = 250,
    } )
end

external_setter({
        item_id = user_codes_item_id,
        key = "4",
        value_type = "user_code",
        value = {
            name = "Artem",
            code = "2222"
        }
    })

send_user_code("enabled", 4, "2222")


external_setter({
        item_id = user_codes_item_id,
        key = "50",
        value_type = "user_code",
        value = {
            name = "Vladimir",
            code = "1234"
        }
    })

send_user_code("enabled", 50, "1234")

local expected_user_codes = {
    ["4"] = { name = "Artem", code = "2222", mode = "enabled" },
    ["50"] = { name = "Vladimir", code = "1234", mode = "enabled" } 
}

tests.assert_eq( core.get_item(user_codes_item_id).value, expected_user_codes )


local run_migration = loadfile("HUB:zwave/scripts/tests/migration_test_suite/run_migration")

local default_changes = require("HUB:zwave/scripts/migration/version_changes/default")
default_changes.check_all_nodes = true
loadfile = loadfile("HUB:zwave/scripts/tests/migration_test_suite/setup_versions")(require "storage", default_changes, 1, 2)

run_migration()

tests.assert_eq( core.get_item(user_codes_item_id).value, expected_user_codes )
