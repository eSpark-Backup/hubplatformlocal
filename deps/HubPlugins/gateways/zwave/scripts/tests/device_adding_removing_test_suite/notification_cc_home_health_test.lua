local zwave = require("zwave")
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local item_descriptor_tags = require("HUB:zwave/scripts/resources/item_descriptor_tags")
local tests = require("tests")

zwave.request_node_values = tests.create_mock()

local cc_notification = {
    id = class_ids.notification,
    notifications = {
        {
            type = "home_health",
            event_types = { 
                "leaving_bed",
                "sitting_on_bed",
                "lying_on_bed",
                "sitting_on_bed_edge",
                "posture_changed",
                "volatile_organic_compound_level",
                "sleep_apnea_detected",
                "sleep_stage_0_detected",
                "sleep_stage_1_detected",
                "sleep_stage_2_detected",
                "sleep_stage_3_detected",
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

local sleep_stage_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/sleep_stage")
local voc_level_status_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/voc_level_status")
local sleep_apnea_status_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/sleep_apnea_status")

-- check adding result
local expected_items = {
    {
        name = "position",
        tag = item_descriptor_tags.home_health,
        cc_id = class_ids.notification,
        channel_id = 0,
        value_type = "token",
        enum = {
            "unknown",
            "leaving_bed",
            "sitting_on_bed",
            "lying_on_bed",
            "sitting_on_bed_edge"
        },
        value = "unknown"
    },
        {
        name = "sleep_stage",
        tag = item_descriptor_tags.home_health,
        cc_id = class_ids.notification,
        channel_id = 0,
        value_type = "token",
        enum = {
            sleep_stage_enum.unknown,
            sleep_stage_enum.not_sleeping,
            sleep_stage_enum.dreaming,
            sleep_stage_enum.light_sleep,
            sleep_stage_enum.medium_sleep,
            sleep_stage_enum.deep_sleep,
        },
        value = sleep_stage_enum.unknown
    },
    {
        name = "voc_level_status",
        tag = item_descriptor_tags.home_health,
        cc_id = class_ids.notification,
        channel_id = 0,
        value_type = "token",
        enum = {
            voc_level_status_enum.unknown,
            voc_level_status_enum.clean,
            voc_level_status_enum.slightly_poluted,
            voc_level_status_enum.moderately_polluted,
            voc_level_status_enum.highly_polluted,
        },
        value = voc_level_status_enum.unknown
    },
    {
        name = "sleep_apnea",
        tag = item_descriptor_tags.home_health,
        cc_id = class_ids.notification,
        channel_id = 0,
        value_type = "token",
        enum = {
            sleep_apnea_status_enum.unknown,
            sleep_apnea_status_enum.clean,
            sleep_apnea_status_enum.slightly_poluted,
            sleep_apnea_status_enum.moderately_polluted,
        },
        value = sleep_apnea_status_enum.unknown
    }
}

local expected_devices = {
    {
        name = "Home Health",
        category = "generic_sensor",
        subcategory = "",
        items = expected_items,
        node_id = "9000",
        battery_powered = true
    }
}

local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"
loadfile( test_suite_dir .. "check_zwave_node_adding_result" )( expected_devices )

