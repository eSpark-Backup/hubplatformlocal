local tests = require("tests")

local cluster_level_control = {
    name = "level_control",
    attributes = {
        {
            _id = 0,
            cluster_id = 8,
            formatted_value = 254,
            is_mfg_specific = false,
            mfg_code = 65535,
            name = "CurrentLevel",
            type = "uint8_t",
            value = "254"
        },
        {
            _id = 65533,
            cluster_id = 8,
            formatted_value = "2",
            is_mfg_specific = false,
            mfg_code = 65535,
            name = "unknown",
            type = "uint16_t",
            value = "2"
        }
    },
    commands = {
        {
            _id = 0,
            cluster_id = 8,
            is_mfg_specific = false,
            mfg_code = 65535,
            name = "MoveToLevel",
            type = "received"
        },
        {
            _id = 1,
            cluster_id = 8,
            is_mfg_specific = false,
            mfg_code = 65535,
            name = "Move",
            type = "received"
        },
        {
            _id = 2,
            cluster_id = 8,
            is_mfg_specific = false,
            mfg_code = 65535,
            name = "Step",
            type = "received"
        },
        {
            _id = 3,
            cluster_id = 8,
            is_mfg_specific = false,
            mfg_code = 65535,
            name = "Stop",
            type = "received"
        },
        {
            _id = 4,
            cluster_id = 8,
            is_mfg_specific = false,
            mfg_code = 65535,
            name = "MoveToLevelWithOnOff",
            type = "received"
        },
        {
            _id = 5,
            cluster_id = 8,
            is_mfg_specific = false,
            mfg_code = 65535,
            name = "MoveWithOnOff",
            type = "received"
        },
        {
            _id = 6,
            cluster_id = 8,
            is_mfg_specific = false,
            mfg_code = 65535,
            name = "StepWithOnOff",
            type = "received"
        },
        {
            _id = 7,
            cluster_id = 8,
            is_mfg_specific = false,
            mfg_code = 65535,
            name = "StopWithOnOff",
            type = "received"
        },
        {
            _id = 160,
            cluster_id = 8,
            is_mfg_specific = false,
            mfg_code = 65535,
            name = "Unknown",
            type = "received"
        },
        {
            _id = 175,
            cluster_id = 8,
            is_mfg_specific = false,
            mfg_code = 65535,
            name = "Unknown",
            type = "received"
        }
    }
}

local cluster_on_off = {
    name = "on_off",
    attributes = {
        {
            _id = 0,
            cluster_id = 6,
            formatted_value = "true",
            is_mfg_specific = false,
            mfg_code = 65535,
            name = "OnOff",
            type = "bool",
            value = "true"
        },
        {
            _id = 65533,
            cluster_id = 6,
            formatted_value = "2",
            is_mfg_specific = false,
            mfg_code = 65535,
            name = "unknown",
            type = "uint16_t",
            value = "2"
        }
    },
    commands = {
        {
            _id = 0,
            cluster_id = 6,
            is_mfg_specific = false,
            mfg_code = 65535,
            name = "Off",
            type = "received"
        },
        {
            _id = 1,
            cluster_id = 6,
            is_mfg_specific = false,
            mfg_code = 65535,
            name = "On",
            type = "received"
        },
        {
            _id = 2,
            cluster_id = 6,
            is_mfg_specific = false,
            mfg_code = 65535,
            name = "Toggle",
            type = "received"
        }
    }
}

local cluster_power_configuration = {
    name = "power_configuration",
    attributes = {
        {
            _id = 33,
            cluster_id = 1,
            formatted_value = 60,
            is_mfg_specific = false,
            mfg_code = 65535,
            name = "BatteryPercentageRemaining",
            type = "uint8_t",
            value = "60"
        },
        {
            _id = 65533,
            cluster_id = 1,
            formatted_value = "1",
            is_mfg_specific = false,
            mfg_code = 65535,
            name = "unknown",
            type = "uint16_t",
            value = "1"
        }
    },
    commands = {}
}

local endpoint = {
    id = 1,
    clusters = {
        cluster_level_control,
        cluster_on_off,
        cluster_power_configuration
    }
}

local node = {
    id = 5,
    manufacturer_id = "AXIS",
    model_id = "Gear",
    security = false,
    is_reachable = false,
    is_battery_powered = true,
    endpoints = {endpoint}
}

local events_handler = assert(loadfile("HUB:zigbee/scripts/events/zigbee/events_handling"))

local zigbee = require("zigbee")

-- prepare zigbee
zigbee.add_node(node)

-- add node
events_handler(
    {
        event = "node_added",
        node_id = 5
    }
)

-- check adding result

local expected_items = {
    {
        name = "battery",
        endpoint_id = 1,
        cluster_name = "power_configuration",
        attribute_name = "BatteryPercentageRemaining",
        value_type = "int",
        value = 100
    },
    {
        name = "dimmer",
        endpoint_id = 1,
        cluster_name = "level_control",
        attribute_name = "CurrentLevel",
        value_type = "int",
        maxValue = 100,
        minValue = 0,
        value = 0
    },
    {
        name = "dimmer_down",
        endpoint_id = 1,
        cluster_name = "level_control",
        attribute_name = "CurrentLevel",
        value_type = "int",
        value = 0
    },
    {
        name = "dimmer_up",
        endpoint_id = 1,
        cluster_name = "level_control",
        attribute_name = "CurrentLevel",
        value_type = "int",
        value = 0
    },
    {
        name = "dimmer_stop",
        endpoint_id = 1,
        cluster_name = "level_control",
        attribute_name = "CurrentLevel",
        value_type = "int",
        value = 0
    },
    {
        name = "switch",
        endpoint_id = 1,
        cluster_name = "on_off",
        attribute_name = "OnOff",
        value_type = "bool",
        value = false
    }
}

local expected_device = {
    {
        name = "Roller Shutter",
        category = "window_cov",
        subcategory = "window_cov",
        items = expected_items,
        node_id = 5,
        battery_powered = true
    }
}

local check_context = {
    descriptors = {},
    item_ids = {},
    device_ids = {}
}

local test_suite_dir = "HUB:zigbee/scripts/tests/device_adding_removing_test_suite/"

loadfile(test_suite_dir .. "check_zigbee_node_adding_result")(expected_device, check_context)

-- remove node
events_handler(
    {
        event = "node_removed",
        node_id = 5,
        manufacturer_id = "AXIS",
        model_id = "Gear"
    }
)

-- check node removing result
loadfile(test_suite_dir .. "check_zigbee_node_removing_result")(check_context)
