local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/panic_button_fixture")()
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local item_names = require( "HUB:zwave/scripts/resources/item_names" )
local item_descriptor_tags = require("HUB:zwave/scripts/resources/item_descriptor_tags")
local tests = require( "tests" )
local zwave = require( "zwave" )
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

zwave.request_node_values = tests.create_mock()

fixture:add_device_with_intrusion_alarm(
    {
        panic_triggers = {
            {button_number = 2, panic = { "held_down" }},
            {item_name = "intrusion_alarm", panic = { "intrusion" }},
            {item_name = "tampering_impact_alarm", panic = { "impact_detected" }}
        },
        panic_button_policy = "separate_device",
        name = "Scene controller",
        category = "scene_controller",
        subcategory = "generic"
    }
)

local intrusion_alarm_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/intrusion_alarm_event")
local tampering_impact_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/tampering_impact_events")

local function is_security_threat_merged(main_item_name, class_id, tag)
    local item_id = storage:call("find_item_id", 
                                main_item_name, 
                                9000, 
                                class_id, 
                                0, 
                                tag)
                                
    local main_descriptor = storage:call("find_item_descriptor", item_id)

    return main_descriptor.item_name ~= main_item_name or main_descriptor.tag ~= tag
end

local expected_devices = {
    {
        name = "Scene controller",
        category = "scene_controller",
        subcategory = "generic",
        items = {
            {
                name = "button_state",
                value_type = "button_state",
                enum = {"press_1_time", "released", "held_down", "idle"},
                tag = "1",
                value = {button_number = 1, button_state = "idle"},
                cc_id = class_ids.central_scene,
                channel_id = 0
            },
            {
                name = "button_state",
                value_type = "button_state",
                enum = {"press_1_time", "released", "held_down", "idle"},
                tag = "3",
                value = {button_number = 3, button_state = "idle"},
                cc_id = class_ids.central_scene,
                channel_id = 0
            },
        },
        node_id = "9000",
        battery_powered = true,
        security = "no",

        subdevices = {
            {
                name = "Panic Button",
                category = "scene_controller",
                subcategory = "panic",
                items = {
                    {
                        name = "button_state",
                        value_type = "button_state",
                        enum = {"press_1_time", "released", "held_down", "idle"},
                        tag = "2",
                        value = {button_number = 2, button_state = "idle"},
                        cc_id = class_ids.central_scene,
                        channel_id = 0
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
                        name = "security_threat",
                        tag = "2.security_tag",
                        cc_id = class_ids.central_scene,
                        channel_id = 0,
                        main_item_name = "button_state",
                        is_merged = is_security_threat_merged(
                            "button_state", 
                            class_ids.central_scene, 
                            "2.security_tag"
                        )
                    },
                    {
                        name = "security_threat",
                        tag = "home_security.security_tag",
                        cc_id = class_ids.notification,
                        channel_id = 0,
                        main_item_name = item_names.intrusion_alarm,
                        is_merged = is_security_threat_merged(
                            item_names.intrusion_alarm, 
                            class_ids.notification, 
                            "home_security.security_tag"
                        )
                    },
                    {
                        name = "security_threat",
                        tag = "home_security.security_tag",
                        cc_id = class_ids.notification,
                        channel_id = 0,
                        main_item_name = item_names.tampering_impact_alarm,
                        is_merged = is_security_threat_merged(
                            item_names.tampering_impact_alarm, 
                            class_ids.notification, 
                            "home_security.security_tag"
                        )
                    },
                }
            }
        }
    }
}

local check_context = {
    descriptors = {},
    item_ids = {},
    device_ids = {},
    tests = tests
}

local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"
loadfile(test_suite_dir .. "check_zwave_node_adding_result")(expected_devices, check_context)

fixture:remove_device_with_intrusion_alarm()

loadfile(test_suite_dir .. "check_zwave_node_removing_result")(check_context)
