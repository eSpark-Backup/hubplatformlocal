local tests = require("tests")
local zwave = require("zwave")
local timer = require("timer")
local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local events_handler = loadfile("HUB:zwave/scripts/events/events_handling")

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

zwave.request_node_values = tests.create_mock()
zwave.request_value = tests.create_mock()

timer.set_timeout_with_id = tests.create_mock()
zwave.set_value = tests.create_mock()

local channel_0 = {
    id = 0,
    classes = {
        { id = class_ids.switch_binary }
    }
}

local channel_1 = {
    id = 1,
    classes = {
        { id = class_ids.switch_binary }
    }
}

local channel_2 = {
    id = 2,
    classes = {
        { id = class_ids.switch_binary }
    }
}

local node = {
    id = 9000,
    manufacturer_id = 99,
    product_type = 20562,
    product_id = 12594,
    is_battery_powered = true,

    specific_type = "",
    generic_type = "",

    security = { "none" },
    security_class_started = { "none" },

    channels = { channel_0, channel_1, channel_2 }
}

-- prepare zwave
zwave.add_node(node)

-- add node
events_handler({
    event = "node_added",
    node_id = 9000
})

local item_names = require("HUB:zwave/scripts/resources/item_names")

-- send motion basic event

local switch_id_0 = storage:call("find_item_id", item_names.switch, 9000, class_ids.switch_binary, 0)
local switch_id_1 = storage:call("find_item_id", item_names.switch, 9000, class_ids.switch_binary, 1)
local switch_id_2 = storage:call("find_item_id", item_names.switch, 9000, class_ids.switch_binary, 2)


local function get_timeout_params(channel_id)
    return {
        tests.skipped_param,
        hubplugins.object("request_value_timer", "HUB:zwave/scripts/timer_handlers", 9000, channel_id, class_ids.switch_binary)._timer_id,
        "HUB:zwave/scripts/timer_handlers/common/request_value",
        {
            node_id = 9000,
            channel_id = channel_id,
            class_id = class_ids.switch_binary,
        }
    }
end

-- set switch item value on main channel
do
    hubplugins.weak_loadfile("HUB:zwave/scripts/set_item_value")({ item_id = switch_id_0, value = true })

    tests.assert_called(zwave.set_value, "check switch value set to zwave", {
        { 9000, class_ids.switch_binary, 0, true }
    })

    tests.assert_called(timer.set_timeout_with_id, "check request switch values", {
        get_timeout_params(1),
        get_timeout_params(2)
    })
end

-- set switch item value on non main channel
do
    hubplugins.weak_loadfile("HUB:zwave/scripts/set_item_value")({ item_id = switch_id_2, value = true })

    tests.assert_called(zwave.set_value, "check switch value set to zwave", {
        { 9000, class_ids.switch_binary, 2, true }
    })

    tests.assert_called(zwave.request_value, "check request switch values", {
        { 9000, class_ids.switch_binary, 2 }
    })
end
