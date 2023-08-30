local zwave = require("zwave")
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local item_names = require( "HUB:zwave/scripts/resources/item_names" )
local item_descriptor_tags = require("HUB:zwave/scripts/resources/item_descriptor_tags")
local tests = require("tests")
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

zwave.request_node_values = tests.create_mock()

local cc_notification = {
    id = class_ids.notification,
    notifications = {
        {
            type = "home_security",
            event_types = { 
                "intrusion",
                "product_cover_removed",
                "invalid_code",
           --     "glass_breakage",
           --     "motion_detected"
                "product_moved",
                "impact_detected"
            }
        },
    }
}

local channel_0 = { 
    id = 0,
    classes = { cc_notification }
}

local node = {
    id = "9000",
    manufacturer_id = 0,
    product_type = 0,
    product_id = 0,
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

local tampering_cover_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/tampering_cover_events")
local intrusion_alarm_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/intrusion_alarm_event")
local invalid_code_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/tampering_invalid_code_alarm_event")
local tampering_move_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/tampering_move_events")
local tampering_impact_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/tampering_impact_events")

local function is_security_threat_merged(main_item_name)
    local item_id = storage:call("find_item_id", 
                                main_item_name, 
                                9000, 
                                class_ids.notification, 
                                0, 
                                "home_security.security_tag")

    return storage:call("find_item_descriptor", item_id).item_name ~= main_item_name
end

-- check adding result
local expected_items = {
    {
        name = item_names.tampering_cover_alarm,
        tag = item_descriptor_tags.home_security,
        cc_id = class_ids.notification,
        channel_id = 0,
        value_type = "token",
        enum = {
            tampering_cover_enum.unknown,
            tampering_cover_enum.no_tampering_cover,
            tampering_cover_enum.tampering_cover
        },
        value = tampering_cover_enum.unknown
    },
    {
        name = item_names.intrusion_alarm,
        tag = item_descriptor_tags.home_security,
        cc_id = class_ids.notification,
        channel_id = 0,
        value_type = "token",
        enum = {
            intrusion_alarm_enum.unknown,
            intrusion_alarm_enum.intrusion,
            intrusion_alarm_enum.no_intrusion
        },
        value = intrusion_alarm_enum.unknown
    },
    {       
        name = item_names.security_threat,
        tag = "home_security.security_tag",
        cc_id = class_ids.notification,
        channel_id = 0,
        main_item_name = item_names.intrusion_alarm,
        is_merged = is_security_threat_merged(item_names.intrusion_alarm)
    },
    {       
        name = item_names.tampering_invalid_code_alarm,
        tag = item_descriptor_tags.home_security,
        cc_id = class_ids.notification,
        channel_id = 0,
        value_type = "token",
        enum = {
            invalid_code_enum.unknown,
            invalid_code_enum.invalid_code,
            invalid_code_enum.no_invalid_code
        },
        value = invalid_code_enum.unknown
    },
    {       
        name = item_names.tampering_impact_alarm,
        tag = item_descriptor_tags.home_security,
        cc_id = class_ids.notification,
        channel_id = 0,
        value_type = "token",
        enum = {
            tampering_impact_enum.unknown,
            tampering_impact_enum.impact_detected,
            tampering_impact_enum.no_impact
        },
        value = tampering_impact_enum.unknown
    },
    {       
        name = item_names.security_threat,
        tag = "home_security.security_tag",
        cc_id = class_ids.notification,
        channel_id = 0,
        main_item_name = item_names.tampering_impact_alarm,
        is_merged = is_security_threat_merged(item_names.tampering_impact_alarm)
    },
    {       
        name = item_names.tampering_move_alarm,
        tag = item_descriptor_tags.home_security,
        cc_id = class_ids.notification,
        channel_id = 0,
        value_type = "token",
        enum = {
            tampering_move_enum.unknown,
            tampering_move_enum.product_moved,
            tampering_move_enum.no_moving
        },
        value = tampering_move_enum.unknown
    }
}

local expected_devices = {
    {
        name = "Unknown",
        category = "unknown",
        items = expected_items,
        node_id = "9000",
        battery_powered = true
    }
}

local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"
loadfile( test_suite_dir .. "check_zwave_node_adding_result" )( expected_devices )

