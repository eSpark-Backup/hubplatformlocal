local fixture = {}

local core = require("core")
local storage = require("storage")
local zwave = require("zwave")
local tests = require("tests")

local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local storage_helper = hubplugins.object( "storage_helper", "HUB:zwave/scripts/utils")

function fixture.init()
    zwave.request_node_values = tests.create_mock()
end

function fixture.add_node(associations, additional_channels_associations)
    local cc_switch = {
        id = class_ids.switch_binary,
    }

    local channel_0 = {
        id = 0,
        classes = { cc_switch }
    }

    if(associations) then
        for _, association_cc in ipairs(associations) do
            table.insert(channel_0.classes, association_cc)
        end
    end

    local node = {
        id = "9000",
        manufacturer_id = 1,
        product_type = 2,
        product_id = 3,
        is_battery_powered = true,

        specific_type = "",
        generic_type = "GENERIC_TYPE_SWITCH_BINARY",

        security = { "none" },
        security_class_started = { "none" },

        channels = { channel_0 }
    }

    if additional_channels_associations then
        local channel_id = 1
        for _,association_classes in ipairs(additional_channels_associations) do
            local channel = {id = channel_id, classes = association_classes}
            table.insert(node.channels, channel)
            channel_id = channel_id + 1
        end
    end

    zwave.add_node( node )

    -- add node
    events_handler( {
        event = "node_added",
        node_id = 9000
    } )
end

function fixture.remove_node()
    events_handler( {
        event = "node_removed",
        node_id = "9000",
        manufacturer_id = 1,
        product_type = 2,
        product_id = 3,
    } )
end

function fixture.find_associations_groups_setting()
    local setting_id = storage_helper:call("find_setting_id", 9000, nil, "associations_groups")
    if setting_id then return core.get_setting(setting_id) end
end

function fixture.find_associations_setting()
    local setting_id = storage_helper:call("find_setting_id", 9000, nil, "associations")
    if setting_id then return core.get_setting(setting_id) end
end

function fixture.check_settings_not_added()
    tests.assert_eq(fixture.find_associations_groups_setting(), nil)
    tests.assert_eq(fixture.find_associations_setting(), nil)
end

function fixture.check_group_setting_static_fields(setting)
    tests.assert_eq(setting.status, "synced", "Check setting status")
    tests.assert_eq(setting.label, {text = "Zwave Association Groups"}, "Check setting label")
    tests.assert_eq(
        setting.description,
        {text = "Represents list of supported zwave association groups"},
        "Check setting description"
    )
    tests.assert_eq(setting.value_type, "dictionary.association_group", "Check setting value_type")
end

function fixture.check_associations_setting_static_fields(setting)
    tests.assert_eq(setting.status, "synced", "Check setting status")
    tests.assert_eq(setting.label, {text = "Zwave Associations"}, "Check setting label")
    tests.assert_eq(
        setting.description,
        {text = "Setting to control device associations"},
        "Check setting description"
    )
    tests.assert_eq(setting.value_type, "dictionary.array.association", "Check setting value_type")
    tests.assert_eq(setting.value, {}, "Check setting value")
end

function fixture.generate_associations_cc(cc_id, groups)
    return {
        association_groups = groups,
        association_groups_count = #groups,
        id = cc_id,
        version = 3
    }
end

return fixture
