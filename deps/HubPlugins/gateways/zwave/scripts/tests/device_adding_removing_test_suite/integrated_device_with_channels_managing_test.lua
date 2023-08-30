local zwave = require("zwave")
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local tests = require "tests"
local core = require( "core" )
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

tests.override_function(_G, "loadfile", function(orig_loadfile, path)
    if path == "HUB:zwave/scripts/model/devices/devices_info/generated/999_133_1" then
        return function()
            local device_info = orig_loadfile("HUB:zwave/scripts/model/devices/devices_info/base_info")()
            device_info.name = 'Test Device'
            device_info.type = 'main_type'
            device_info.category = 'main_category'
            device_info.subcategory = 'main_subcategory'
            device_info.info = {manufacturer = 'Test', model = 'channels test'}
            device_info.endpoints_info = {}
            device_info.endpoints_info[1] = {name = 'First Channel Subdevice', type = 'first_channell_type', category = 'first_channel_category', subcategory = 'first_channel_subcategory'}
            device_info.endpoints_info[2] = {merge_with = 1, category = 'unused category from 2nd channel'}
            device_info.endpoints_info[3] = {ignored = true, category = 'unused category from 3rd channel'}
            device_info.endpoints_info[5] = {merge_with = 1, category = 'unused category from 5th channel'}
            device_info.endpoints_info[6] = {readonly = true, name = 'Readonly Channel Subdevice', type = 'readonly_channel_type', category = 'readonly_channel_category', subcategory = 'readonly_channel_subcategory'}

            return device_info
        end
    else
        return orig_loadfile(path)
    end
end)

zwave.request_node_values = tests.create_mock()

local function make_channel(id)
    return { 
        id = id,
        classes = { 
            { id = class_ids.switch_binary }
        }
    }
end

local node = {
    id = "9000",
    manufacturer_id = 999,
    product_type = 133,
    product_id = 1,
    is_battery_powered = false,

    specific_type = "SPECIFIC_TYPE_POWER_SWITCH_BINARY",
    generic_type = "GENERIC_TYPE_SWITCH_BINARY",

    security = { "none" },
    security_class_started = { "none" },

    channels = { 
        make_channel(0),
        make_channel(1),
        make_channel(2),
        make_channel(3),
        make_channel(4),
        make_channel(5),
        make_channel(6)
    }
}

zwave.add_node( node )

-- add node
events_handler( {
    event = "node_added",
    node_id = 9000 
} )

-- check adding result


local function make_switch_item(channel_id, merged, readonly)
    return {
        name = "switch",
        cc_id = class_ids.switch_binary,
        channel_id = channel_id,
        is_merged = merged,
        has_setter = not readonly
    }
end

local expected_devices = {
    {
        name = "Test Device",
        category = "main_category",
        subcategory = "main_subcategory",
        items = {
            make_switch_item(0)
        },
        node_id = "9000",
        battery_powered = false,
        subdevices = {
            {
                name = "First Channel Subdevice",
                category = "first_channel_category",
                subcategory = "first_channel_subcategory",
                items = {
                    make_switch_item(1),
                    make_switch_item(2, true),
                    make_switch_item(5, true)
                }
            },
            {
                name = "Test Device 1",
                category = "main_category",
                subcategory = "main_subcategory",
                items = {
                    make_switch_item(4)
                }
            },
            {
                name = "Readonly Channel Subdevice",
                category = "readonly_channel_category",
                subcategory = "readonly_channel_subcategory",
                items = {
                    make_switch_item(6, false, true)
                }
            }
        }
    }
}

local check_context = {}

local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"

loadfile( test_suite_dir .. "check_zwave_node_adding_result" )( expected_devices, check_context )

local item_names = require("HUB:zwave/scripts/resources/item_names")
local readonly_item = core.get_item(storage:call("find_item_id", item_names.switch, node.id, class_ids.switch_binary, 6))
tests.assert_ne(readonly_item, nil, "Binary Switch item for readonly channel must be created")
tests.assert_eq(readonly_item.has_setter, false, "Readonly item must not have a setter")
