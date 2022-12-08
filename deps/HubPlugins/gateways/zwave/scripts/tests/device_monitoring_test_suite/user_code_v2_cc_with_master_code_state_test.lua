local zwave = require("zwave")
local core = require("core")
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local item_value_type = require("HUB:zwave/scripts/resources/item_value_type")
local tests = require("tests")
local item_names = require("HUB:zwave/scripts/resources/item_names")

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local master_code_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/master_code_state")

zwave.request_node_values = tests.create_mock()

local cc_user_code = {
    id = class_ids.user_codes,
    supported_users_count = 250,
    version = 2,
    capabilities = {
        checksum_supported = true,
        master_code_supported = true,
        master_code_deactivation_supported = true,
        supported_keys = "0123456789",
        supported_keypad_modes = { },
        supported_user_id_statuses = { }
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

local function get_item_value(item_name)
    local id = storage:call("find_item_id", 
                            item_name, 
                            "9000", 
                            class_ids.user_codes,
                            1)
    assert(id)
    return core.get_item(id).value
end

-- update values

do
    events_handler( {
        event = "master_code_updated",
        node_id = "9000",
        channel_id = 1,
        class_id = class_ids.user_codes,
        value = "1234"
    } )

    tests.assert_eq( get_item_value(item_names.master_code), "1234", "Check master code after set" )
    tests.assert_eq( get_item_value(item_names.master_code_state), master_code_enum.enabled, "Check master code state after set" )
end

do
    events_handler( {
        event = "master_code_updated",
        node_id = "9000",
        channel_id = 1,
        class_id = class_ids.user_codes,
        value = ""
    } )

    tests.assert_eq( get_item_value(item_names.master_code), "1234", "Check master code after deactivation" )
    tests.assert_eq( get_item_value(item_names.master_code_state), master_code_enum.disabled, "Check master code state after deactivation" )
end

do
    events_handler( {
        event = "master_code_updated",
        node_id = "9000",
        channel_id = 1,
        class_id = class_ids.user_codes,
        value = "7890"
    } )

    tests.assert_eq( get_item_value(item_names.master_code), "7890", "Check master code after set again" )
    tests.assert_eq( get_item_value(item_names.master_code_state), master_code_enum.enabled, "Check master code state after set again" )
end


