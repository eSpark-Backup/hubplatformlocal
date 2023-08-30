local zwave = require("zwave")
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local item_value_type = require("HUB:zwave/scripts/resources/item_value_type")
local item_names = require("HUB:zwave/scripts/resources/item_names")
local tests = require("tests")
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

zwave.request_node_values = tests.create_mock()

local function test_impl( params, has_anti_theft_unlock_cc )
    local channel_0 = {
        id = 0,
        classes = { 
            {
                version = 3,
                id = class_ids.anti_theft,
                initial_lock_info = params.manufacturer_id and {
                    restricted = params.restricted,
                    hint = "A7901C9D",
                    manufacturer_id = params.manufacturer_id,
                    zwave_alliance_locking_id = 500
                }
            }
        }
    }

    if has_anti_theft_unlock_cc then
        table.insert( channel_0.classes, {
            version = 3,
            id = class_ids.anti_theft_unlock,
            initial_lock_info = params.manufacturer_id and {
                restricted = params.restricted,
                hint = "A7901C9D",
                manufacturer_id = params.manufacturer_id,
                zwave_alliance_locking_id = 500
            }
        } )
    end


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


    -- check adding result

    local expected_settings = {
        {
            value_type = "action",
            value = {
                text = "Rediscover",
                lang_tag = "rediscover_tag"
            },
            tag = "rediscover"
        }
    }

    local specific_setting_expectations = {
        expect_auto_unlock_setting = {
            value_type = "action",
            tag = "auto_unlock",
            cc_id = class_ids.anti_theft,
            value = { 
                text = "Auto Unlock" 
            }
        },
        expect_password_setting =  {
            value_type = "string",
            value = "",
            tag = "password",
            cc_id = class_ids.anti_theft
        },
        expect_lock_info_setting = {
            value_type = "dictionary.string",
            value = {
                hint = "A7901C9D",
                manufacturer = params.manufacturer_id and ( "manufacturer_" .. params.manufacturer_id ),
                zwave_alliance_locking_id = "500"
            },
            tag = "lock_info",
            cc_id = class_ids.anti_theft
        },
        expect_anti_theft_enabled_setting = {
            value_type = "bool",
            value = params.manufacturer_id ~= nil,
            value_default = false,
            tag = "anti_theft_enabled",
            cc_id = class_ids.anti_theft  
        }
    }

    for expectation, setting_info in pairs(specific_setting_expectations) do
        if params[expectation] then
            table.insert(expected_settings, setting_info)
        end
    end


    local expected_devices = {
        {
            name = "Unknown",
            category = "unknown",
            items = { },
            settings = expected_settings,
            node_id = "9000",
            battery_powered = true
        }
    }

    local check_context = {}

    local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"

    hubplugins.weak_loadfile( test_suite_dir .. "check_zwave_node_adding_result" )( expected_devices, check_context )


    -- remove node
    events_handler( {
        event = "node_removed",
        node_id = "9000",
        manufacturer_id = 0,
        product_type = 1,
        product_id = 2,
    } )

    -- check removing result
    hubplugins.weak_loadfile( test_suite_dir .. "check_zwave_node_removing_result" )( check_context )
end


tests.run_with_params( test_impl, 
{
    {
        value = { 
            manufacturer_id = 603,
            restricted = true, 

            expect_auto_unlock_setting = true,
            expect_password_setting = true,
            expect_lock_info_setting = true,
            expect_anti_theft_enabled_setting = false
        },
        description = "Locked by Ezlo manufacturer"
    },
    {
        value = { 
            manufacturer_id = 602,
            restricted = true,
            
            expect_auto_unlock_setting = false,
            expect_password_setting = true,
            expect_lock_info_setting = true,
            expect_anti_theft_enabled_setting = false
        },
        description = "locked by some manufacturer"
    },
    {
        value = { 
            manufacturer_id = 603,
            restricted = false,
            
            expect_auto_unlock_setting = false,
            expect_password_setting = false,
            expect_lock_info_setting = false,
            expect_anti_theft_enabled_setting = true
        },
        description = "Locked by Ezlo manufacturer and not restricted"
    },
    {
        value = { 
            manufacturer_id = 604,
            restricted = false,
            
            expect_auto_unlock_setting = false,
            expect_password_setting = true,
            expect_lock_info_setting = true,
            expect_anti_theft_enabled_setting = false
        },
        description = "Locked by some manufacturer and not restricted"
    },
    {
        value = { 
            expect_auto_unlock_setting = false,
            expect_password_setting = false,
            expect_lock_info_setting = false,
            expect_anti_theft_enabled_setting = true
        },
        description = "Not locked"
    }
},
{
    {
        value = true,
        description = "Anti-theft unlock present"
    },
    {
        value = false,
        description = "lAnti-theft unlock absent"
    },
})
