local zwave = require("zwave")
local core = require("core")
local storage = require( "storage" )
local tests = require("tests")
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"

zwave.request_node_values = tests.create_mock()

local default_changes = loadfile("HUB:zwave/scripts/migration/version_changes/default")()
default_changes.check_all_nodes = true
loadfile = loadfile( "HUB:zwave/scripts/tests/migration_test_suite/setup_versions")(
    storage,
    default_changes,
    1,
    2
)

local create_default_settings_impl
local function create_default_settings(...)
    return create_default_settings_impl(...)
end

tests.override_function(_G, "loadfile", function(orig, path)
    local result = orig(path)

    if path == "HUB:zwave/scripts/model/devices/inclusion/default/create_default_settings" then
        create_default_settings_impl = result
        return create_default_settings
    else
        return result
    end
end)

--prepare zwave node
local node = loadfile( "HUB:zwave/scripts/tests/migration_test_suite/simple_zwave_node_resource" )()
zwave.add_node( node )

-- add node
events_handler( {
    event = "node_added",
    node_id = 9000
} )

-- check adding result

local rediscover_setting = {
    label = {
        text = "Rediscover device"
    },
    description = {
        text = "Rediscover (repeat interview) device"
    },
    value_type = "action",
    value = {
        text = "Rediscover",
        lang_tag = "rediscover_tag"
    },
    tag = "rediscover",
    descriptor = {
        node_id = "9000",
        tag = "rediscover"
    }
}

local expected_devices = {
    {
        name = "Water Valve",
        category = "switch",
        subcategory = "valve",
        node_id = "9000",
        battery_powered = true,
        items = {
            {
                name = "switch",
                cc_id = class_ids.switch_binary,
                channel_id = 0
            }
        },
        settings = {
            rediscover_setting
        }
    }
}

loadfile( test_suite_dir .. "check_zwave_node_adding_result" )( expected_devices )

-- modify zwave node

rediscover_setting.label.text = "hello"

local create_default_settings_original = create_default_settings_impl
create_default_settings_impl = function(self, device_data, zwave_node_id)
    table.insert(device_data.settings, rediscover_setting)
end

-- execute migration
loadfile("HUB:zwave/scripts/tests/migration_test_suite/run_migration")()

-- check migration result
loadfile( test_suite_dir .. "check_zwave_node_adding_result" )( expected_devices )
