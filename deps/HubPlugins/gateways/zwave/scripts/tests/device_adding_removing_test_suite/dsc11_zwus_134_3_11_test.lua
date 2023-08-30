local item_names = require("HUB:zwave/scripts/resources/item_names")
local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local item_tags = require("HUB:zwave/scripts/resources/item_descriptor_tags")
local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/dsc11_zwus_134_3_11_fixture")()
local zwave = require("zwave")
local tests = require( "tests" )

zwave.request_node_values = tests.create_mock()

local node = fixture:create_node("9000")

fixture:add_node(node)

local expected_devices = {
    {
        name = "Smart Power Strip",
        category = "switch",
        subcategory = "interior_plugin",
        node_id = "9000",
        battery_powered = false,
        settings = {
            {
                value_type = "action",
                value = {
                    text = "Rediscover",
                    lang_tag = "rediscover_tag"
                },
                tag = "rediscover"
            },
            {
                cc_id = class_ids.configuration,
                value_type = "dictionary.zwave_configuration",
                value = {}
            }
        },
        items = {
            {
                name = item_names.electric_meter_kwh,
                cc_id = class_ids.meter,
                channel_id = 0,
            },
            {
                name = item_names.electric_meter_watt,
                cc_id = class_ids.meter,
                channel_id = 0,
            },
            {
                name = item_names.switch,
                cc_id = class_ids.switch_binary,
                channel_id = 0
            }
        },
        subdevices = {
            {
                name = "Smart Power Strip 1",
                category = "switch",
                subcategory = "interior_plugin",
                items = {
                    {
                        name = item_names.electric_meter_kwh,
                        cc_id = class_ids.meter,
                        channel_id = 3,
                    },
                    {
                        name = item_names.electric_meter_watt,
                        cc_id = class_ids.meter,
                        channel_id = 3,
                    }, 
                    {
                        name = item_names.switch,
                        cc_id = class_ids.switch_binary,
                        channel_id = 1
                    },
                    {
                        name = item_names.switch,
                        cc_id = class_ids.basic,
                        channel_id = 1,
                        is_merged = true
                    }
                }
            },
            {
                name = "Smart Power Strip 2",
                category = "switch",
                subcategory = "interior_plugin",
                items = {
                    {
                        name = item_names.electric_meter_kwh,
                        cc_id = class_ids.meter,
                        channel_id = 4,
                    },
                    {
                        name = item_names.electric_meter_watt,
                        cc_id = class_ids.meter,
                        channel_id = 4,
                    }, 
                    {
                        name = item_names.switch,
                        cc_id = class_ids.switch_binary,
                        channel_id = 2
                    },
                    {
                        name = item_names.switch,
                        cc_id = class_ids.basic,
                        channel_id = 2,
                        is_merged = true
                    }
                }
            },
            {
                name = "Smart Power Strip 3",
                category = "switch",
                subcategory = "interior_plugin",
                items = {
                    {
                        name = item_names.electric_meter_kwh,
                        cc_id = class_ids.meter,
                        channel_id = 5,
                    },
                    {
                        name = item_names.electric_meter_watt,
                        cc_id = class_ids.meter,
                        channel_id = 5,
                    }, 
                    {
                        name = item_names.switch,
                        cc_id = class_ids.switch_binary,
                        channel_id = 3
                    },
                    {
                        name = item_names.switch,
                        cc_id = class_ids.basic,
                        channel_id = 3,
                        is_merged = true
                    }
                }
            },
            {
                name = "Smart Power Strip 4",
                category = "switch",
                subcategory = "interior_plugin",
                items = {
                    {
                        name = item_names.electric_meter_kwh,
                        cc_id = class_ids.meter,
                        channel_id = 6,
                    },
                    {
                        name = item_names.electric_meter_watt,
                        cc_id = class_ids.meter,
                        channel_id = 6,
                    }, 
                    {
                        name = item_names.switch,
                        cc_id = class_ids.switch_binary,
                        channel_id = 4
                    },
                    {
                        name = item_names.switch,
                        cc_id = class_ids.basic,
                        channel_id = 4,
                        is_merged = true
                    }
                }
            },
            {
                name = "Smart Power Strip 5",
                category = "switch",
                subcategory = "interior_plugin",
                items = {
                    {
                        name = item_names.electric_meter_kwh,
                        cc_id = class_ids.meter,
                        channel_id = 1,
                    },
                    {
                        name = item_names.electric_meter_watt,
                        cc_id = class_ids.meter,
                        channel_id = 1,
                    }
                }
            },
            {
                name = "Smart Power Strip 6",
                category = "switch",
                subcategory = "interior_plugin",
                items = {
                    {
                        name = item_names.electric_meter_kwh,
                        cc_id = class_ids.meter,
                        channel_id = 2,
                    },
                    {
                        name = item_names.electric_meter_watt,
                        cc_id = class_ids.meter,
                        channel_id = 2,
                    }
                }
            },
        }
    }
}

local check_context = {}

local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"
loadfile(test_suite_dir .. "check_zwave_node_adding_result")(expected_devices, check_context)

fixture:remove_node(node)

loadfile(test_suite_dir .. "check_zwave_node_removing_result")(check_context)
