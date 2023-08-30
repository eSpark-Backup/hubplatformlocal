local zwave = require("zwave")
local core = require("core")
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local set_item_value = loadfile( "HUB:zwave/scripts/set_item_value" )
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

local function update_master_code(code)
    events_handler( {
        event = "master_code_updated",
        node_id = "9000",
        channel_id = 1,
        class_id = class_ids.user_codes,
        value = code
    } )
end

local function find_item_id(item_name)
    return storage:call("find_item_id", 
                            item_name, 
                            "9000", 
                            class_ids.user_codes,
                            1)
end

local function get_item_value(item_name)
    return core.get_item(find_item_id(item_name)).value
end

zwave.set_master_code = tests.create_mock()
zwave.request_master_code = tests.create_mock()

-- add node
events_handler( {
    event = "node_added",
    node_id = "9000"
} )

-- initial value
update_master_code("12345")

-- set enabled state, previous is also enabled
do
    set_item_value({
            item_id = find_item_id(item_names.master_code_state),
            value = master_code_enum.enabled
        })

    tests.assert_called(zwave.set_master_code, "Check set_master_code called", { { "9000", 1, "12345" } })
    tests.assert_called(zwave.request_master_code, "Check set_master_code called", { { "9000", 1 } })
end

-- set new master code, state is enabled
do
    set_item_value({
            item_id = find_item_id(item_names.master_code),
            value = "012345"
        })

    tests.assert_called(zwave.set_master_code, "Check set_master_code called", { { "9000", 1, "012345" } })
    tests.assert_called(zwave.request_master_code, "Check set_master_code called", { { "9000", 1 } })

    tests.assert_eq( get_item_value(item_names.master_code), "12345", "Master code is still previous" )
    tests.assert_eq( get_item_value(item_names.master_code_state), master_code_enum.enabled, "Master code is enabled" )

    update_master_code("012345")
end

-- set disabled state, previous is enabled
do
    set_item_value({
            item_id = find_item_id(item_names.master_code_state),
            value = master_code_enum.disabled
        })

    tests.assert_called(zwave.set_master_code, "Check set_master_code called", { { "9000", 1, "" } })
    tests.assert_called(zwave.request_master_code, "Check set_master_code called", { { "9000", 1 } })

    tests.assert_eq( get_item_value(item_names.master_code), "012345", "Master code is still previous" )
    tests.assert_eq( get_item_value(item_names.master_code_state), master_code_enum.enabled, "Master code is still enabled" )

    update_master_code("")

    tests.assert_eq( get_item_value(item_names.master_code), "012345", "Master code is still previous" )
    tests.assert_eq( get_item_value(item_names.master_code_state), master_code_enum.disabled, "Master code became disabled" )
end

-- set new master code, state is disabled
do
    set_item_value({
            item_id = find_item_id(item_names.master_code),
            value = "12009"
        })

    tests.assert_called(zwave.set_master_code, "Check set_master_code called", { })
    tests.assert_called(zwave.request_master_code, "Check set_master_code called", { })

    tests.assert_eq( get_item_value(item_names.master_code), "12009", "Master code has been changed immediatelly" )
    tests.assert_eq( get_item_value(item_names.master_code_state), master_code_enum.disabled, "Master code state is disabled" )
end

-- set enabled state, previous state is disabled
do
    set_item_value({
            item_id = find_item_id(item_names.master_code_state),
            value = master_code_enum.enabled
        })

    tests.assert_called(zwave.set_master_code, "Last saved master code must be setted", { { "9000", 1, "12009" } })
    tests.assert_called(zwave.request_master_code, "Master code must be requested", { { "9000", 1 } })

    tests.assert_eq( get_item_value(item_names.master_code), "12009", "Master code is not changed" )
    tests.assert_eq( get_item_value(item_names.master_code_state), master_code_enum.disabled, "Master code state is still disabled" )

    update_master_code("12009")
end
