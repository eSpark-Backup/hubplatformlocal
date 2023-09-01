local class_id, device_info = ...

local tests = require("tests")
local zwave = require("zwave")
local core = require("core")
local timer = require("timer")
local item_names = require("HUB:zwave/scripts/resources/item_names")
local events_handler = loadfile("HUB:zwave/scripts/events/events_handling")
local set_item_value = loadfile("HUB:zwave/scripts/set_item_value")

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

local function to_number(value)
    if type(value) == "number" then
        return value
    elseif type(value) == "boolean" then
        return value and 1 or 0
    end
end

-- Simulate real logic

timer.cancel = tests.create_mock()
timer.set_timeout_with_id = function(delay, id, handler, args)
    hubplugins.weak_loadfile(handler)(args)
end

zwave.request_node_values = function(node_id, delay)
    zwave.request_value(node_id, class_id, 0)
    zwave.request_value(node_id, class_id, 1)
    zwave.request_value(node_id, class_id, 2)
end

-- Mock device

local values = {
    ["9000"] = {
        [0] = { [class_id] = 0 },
        [1] = { [class_id] = 0 },
        [2] = { [class_id] = 0 },
    }
}

zwave.request_value = function(node_id, class_id, channel_id)
    local value = values[node_id][channel_id][class_id]

    events_handler({
        event = "value_updated",
        node_id = node_id,
        class_id = class_id,
        channel_id = channel_id,
        value = value
    })
end

zwave.set_value = function(node_id, class_id, channel_id, value)
    value = to_number(value)

    values[node_id][channel_id][class_id] = value

    if channel_id == 0 then
        values[node_id][1][class_id] = value
        values[node_id][2][class_id] = value
    end
end

-- Add node

zwave.add_node({
    id = "9000",
    manufacturer_id = device_info.manufacturer_id,
    product_type = device_info.product_type,
    product_id = device_info.product_id,
    is_battery_powered = false,

    specific_type = "",
    generic_type = "",

    security = { "none" },
    security_class_started = { "none" },

    channels = {
        {
            id = 0,
            classes = {
                { id = class_id }
            }
        },
        {
            id = 1,
            classes = {
                { id = class_id }
            }
        },
        {
            id = 2,
            classes = {
                { id = class_id }
            }
        },
    }
})

events_handler({
    event = "node_added",
    node_id = "9000"
})

-- Fixture

local switch_item_ids = {
    [0] = storage:call("find_item_id", item_names.switch, "9000", class_id, 0),
    [1] = storage:call("find_item_id", item_names.switch, "9000", class_id, 1),
    [2] = storage:call("find_item_id", item_names.switch, "9000", class_id, 2)
}

local function get_switch_value(channel_id)
    return core.get_item(switch_item_ids[channel_id]).value
end

local function set_switch_value(channel_id, value)
    set_item_value({
        item_id = switch_item_ids[channel_id],
        value = value
    })
end

local function check_switch_values(values)
    for channel_id = 0, 2 do
        tests.assert_eq(get_switch_value(channel_id), values[channel_id], "Check value of channel " .. channel_id)
    end
end

-- Test

do -- Initial
    check_switch_values({ [0] = false, [1] = false, [2] = false })
end

do -- Toggle parent device
    set_switch_value(0, true);  check_switch_values({ [0] = true,  [1] = true,  [2] = true  })
    set_switch_value(0, false); check_switch_values({ [0] = false, [1] = false, [2] = false })
end

do -- Toggle single child
    set_switch_value(1, true);  check_switch_values({ [0] = true,  [1] = true,  [2] = false })
    set_switch_value(1, false); check_switch_values({ [0] = false, [1] = false, [2] = false })
    set_switch_value(2, true);  check_switch_values({ [0] = true,  [1] = false, [2] = true  })
    set_switch_value(2, false); check_switch_values({ [0] = false, [1] = false, [2] = false })
end

do -- Toggle both children
    set_switch_value(1, true);  check_switch_values({ [0] = true,  [1] = true,  [2] = false })
    set_switch_value(2, true);  check_switch_values({ [0] = true,  [1] = true,  [2] = true  })
    set_switch_value(1, false); check_switch_values({ [0] = true,  [1] = false, [2] = true  })
    set_switch_value(2, false); check_switch_values({ [0] = false, [1] = false, [2] = false })
end

do -- Toggle parent after a child
    set_switch_value(1, true);  check_switch_values({ [0] = true,  [1] = true,  [2] = false })
    set_switch_value(0, true);  check_switch_values({ [0] = true,  [1] = true,  [2] = true  })
    set_switch_value(1, false); check_switch_values({ [0] = true,  [1] = false, [2] = true  })
    set_switch_value(0, false); check_switch_values({ [0] = false, [1] = false, [2] = false })
end
