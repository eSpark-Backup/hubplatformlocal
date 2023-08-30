local core = require("core")
local tests = require("tests")

local cluster_basic = {
    attributes = {
        {
            cluster_id = 0,
            formatted_value = "1",
            id = 0,
            is_mfg_specific = false,
            mfg_code = 65535,
            name = "ZCLVersion",
            type = "uint8_t",
            value = "1"
        },
        {
            cluster_id = 0,
            formatted_value = "2",
            id = 1,
            is_mfg_specific = false,
            mfg_code = 65535,
            name = "ApplicationVersion",
            type = "uint8_t",
            value = "2"
        },
        {
            cluster_id = 0,
            formatted_value = "2",
            id = 2,
            is_mfg_specific = false,
            mfg_code = 65535,
            name = "StackVersion",
            type = "uint8_t",
            value = "2"
        },
        {
            cluster_id = 0,
            formatted_value = "2",
            id = 3,
            is_mfg_specific = false,
            mfg_code = 65535,
            name = "HWVersion",
            type = "uint8_t",
            value = "2"
        },
        {
            cluster_id = 0,
            formatted_value = "LUMI",
            id = 4,
            is_mfg_specific = false,
            mfg_code = 65535,
            name = "ManufacturerName",
            type = "string",
            value = "LUMI"
        },
        {
            cluster_id = 0,
            formatted_value = "lumi.remote.b1acn01",
            id = 5,
            is_mfg_specific = false,
            mfg_code = 65535,
            name = "ModelIdentifier",
            type = "string",
            value = "lumi.remote.b1acn01"
        },
        {
            cluster_id = 0,
            formatted_value = "20180525",
            id = 6,
            is_mfg_specific = false,
            mfg_code = 65535,
            name = "DateCode",
            type = "string",
            value = "20180525"
        },
        {
            cluster_id = 0,
            formatted_value = "3",
            id = 7,
            is_mfg_specific = false,
            mfg_code = 65535,
            name = "PowerSource",
            type = "enum8_t",
            value = "3"
        }
    },
    commands = {},
    data_holder = "clientServer",
    endpoint_id = 1,
    id = 0,
    name = "basic"
}

local cluster_identify ={
    attributes = {},
    commands = {},
    data_holder = "server",
    endpoint_id = 1,
    id = 3,
    name = "identify"
}

local cluster_multistate_input_basic = {
    attributes = {
        {
            cluster_id = 18,
            formatted_value = "6",
            id = 74,
            is_mfg_specific = false,
            mfg_code = 65535,
            name = "NumberOfStates",
            type = "uint16_t",
            value = "6"
        },
        {
            cluster_id = 18,
            formatted_value = "false",
            id = 81,
            is_mfg_specific = false,
            mfg_code = 65535,
            name = "OutOfService",
            type = "bool",
            value = "false"
        },
        {
            cluster_id = 18,
            formatted_value = "0",
            id = 85,
            is_mfg_specific = false,
            mfg_code = 65535,
            name = "PresentValue",
            type = "uint16_t",
            value = "0"
        },
        {
            cluster_id = 18,
            formatted_value = "0",
            id = 111,
            is_mfg_specific = false,
            mfg_code = 65535,
            name = "StatusFlags",
            type = "bitmap8_t",
            value = "0"
        }
    },
    commands = {},
    data_holder = "server",
    endpoint_id = 1,
    id = 18,
    name = "multistate_input_basic"
}

local endpoint = {
    id = 1,
    device_id = 259,
    profile_id = 260,
    clusters = {
        cluster_basic,
        cluster_identify,
        cluster_multistate_input_basic
    }
}

local node = {
    id = 5,
    manufacturer_id = "LUMI",
    model_id = "lumi.remote.b1acn01",
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

local button_actions = require("HUB:zigbee/scripts/resources/enums/button_actions")

local storage_helper = hubplugins.object("storage_helper", "HUB:zigbee/scripts/utils")

local button_state_item_id =
    storage_helper:call(
    "find_item_id",
    {
        item_name = "button_state",
        node_id = 5,
        endpoint_id = 1,
        cluster_name = "multistate_input_basic",
        attribute_name = "PresentValue"
    }
)
print("button_state_item_id = ", button_state_item_id)
tests.assert_ne(button_state_item_id, nil, "Try to find item id")

core.update_item_value = tests.create_mock()

local function event_button_state_update(value)
    events_handler( {
        event = "attribute_value_updated",
        node_id = 5,
        endpoint_id = 1,
        cluster_name = "multistate_input_basic",
        attribute = {
            cluster_id = 18,
            formatted_value = value,
            id = 85,
            is_mfg_specific = false,
            name = "PresentValue",
            type = "uint16_t",
            value = value},
        }
    )
end

--check single press detected
event_button_state_update("1")

tests.assert_called(
    core.update_item_value,
    "check if PresentValue value changed",
    {
        { button_state_item_id, { button_number = 1, button_state = button_actions.press_1_time } },
        { button_state_item_id, { button_number = 1, button_state = button_actions.idle } }
    }
)

--check hold down detected
event_button_state_update("0")

tests.assert_called(
    core.update_item_value,
    "check if PresentValue value changed",
    {
        { button_state_item_id, { button_number = 1, button_state = button_actions.held_down } }
    }
)

--check release detected
event_button_state_update("255")

tests.assert_called(
    core.update_item_value,
    "check if PresentValue value changed",
    {
        { button_state_item_id, { button_number = 1, button_state = button_actions.released } },
        { button_state_item_id, { button_number = 1, button_state = button_actions.idle } }
    }
)
