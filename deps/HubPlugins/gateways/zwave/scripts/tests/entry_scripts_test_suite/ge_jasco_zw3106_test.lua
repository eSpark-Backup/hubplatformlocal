local tests = require( "tests" )

local item_names = require("HUB:zwave/scripts/resources/item_names")
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local zwave = require("zwave")
local timer = require("timer")
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local fixture = hubplugins.weak_loadfile( "HUB:zwave/scripts/tests/fixtures/ge_jasco_zw3106_fixture" )()

zwave.request_node_values = tests.create_mock()

timer.set_timeout_with_id = tests.create_mock()
timer.cancel = tests.create_mock()
zwave.set_value = tests.create_mock()


local node = fixture:create_node("9000")

fixture:add_node(node)

-- send motion basic event

local switch_id_0 = storage:call("find_item_id", item_names.switch, 9000, class_ids.dimmer, 0 )
local dimmer_id_0 = storage:call("find_item_id", item_names.dimmer, 9000, class_ids.dimmer, 0 )

local switch_id_1 = storage:call("find_item_id", item_names.switch, 9000, class_ids.dimmer, 1 )
local dimmer_id_1 = storage:call("find_item_id", item_names.dimmer, 9000, class_ids.dimmer, 1 )

local switch_id_2 = storage:call("find_item_id", item_names.switch, 9000, class_ids.dimmer, 2 )
local dimmer_id_2 = storage:call("find_item_id", item_names.dimmer, 9000, class_ids.dimmer, 2 )


local function get_timeout_params(channel_id)
    return { 
        tests.skipped_param, 
        hubplugins.object("request_value_timer", "HUB:zwave/scripts/timer_handlers", 9000, channel_id, class_ids.dimmer)._timer_id,
        "HUB:zwave/scripts/timer_handlers/common/request_value", 
        {
            node_id = "9000",
            channel_id = channel_id,
            class_id = class_ids.dimmer,
        } 
    } 
end

-- set switch item value on main channel
do
    hubplugins.weak_loadfile("HUB:zwave/scripts/set_item_value")({ item_id = switch_id_0, value = true })

    tests.assert_called(zwave.set_value, "check dimmer value set to zwave", { { "9000", class_ids.dimmer, 0, 0xFF } })
    tests.assert_called(timer.set_timeout_with_id, "check request dimmer values", 
        { 
            get_timeout_params(0),
            get_timeout_params(1),
            get_timeout_params(2)
        })
end

-- set dimmer item value on main channel
do
    hubplugins.weak_loadfile("HUB:zwave/scripts/set_item_value")({ item_id = dimmer_id_0, value = 50 })

    tests.assert_called(zwave.set_value, "check dimmer value set to zwave", { { "9000", class_ids.dimmer, 0, 50 } })
    tests.assert_called(timer.set_timeout_with_id, "check request dimmer values", 
        { 
            get_timeout_params(0),
            get_timeout_params(1),
            get_timeout_params(2)
        })
end

-- set switch item value on non main channel
do
    hubplugins.weak_loadfile("HUB:zwave/scripts/set_item_value")({ item_id = switch_id_1, value = true })

    tests.assert_called(zwave.set_value, "check dimmer value set to zwave", { { "9000", class_ids.dimmer, 1, 0xFF } })
    tests.assert_called(timer.set_timeout_with_id, "check request dimmer values",
        {
            get_timeout_params(1)
        })
end

-- set switch dimmer value on non main channel
do
    hubplugins.weak_loadfile("HUB:zwave/scripts/set_item_value")({ item_id = dimmer_id_1, value = 60 })

    tests.assert_called(zwave.set_value, "check dimmer value set to zwave", { { "9000", class_ids.dimmer, 1, 60 } })
    tests.assert_called(timer.set_timeout_with_id, "check request dimmer values",
        {
            get_timeout_params(1)
        })
end

-- set switch item value on non main channel
do
    hubplugins.weak_loadfile("HUB:zwave/scripts/set_item_value")({ item_id = switch_id_2, value = true })

    tests.assert_called(zwave.set_value, "check dimmer value set to zwave", { { "9000", class_ids.dimmer, 2, 0xFF } })
    tests.assert_called(timer.set_timeout_with_id, "check request dimmer values", 
        {
            get_timeout_params(2)
        })
end

-- set switch dimmer value on non main channel
do
    hubplugins.weak_loadfile("HUB:zwave/scripts/set_item_value")({ item_id = dimmer_id_2, value = 20 })

    tests.assert_called(zwave.set_value, "check dimmer value set to zwave", { { "9000", class_ids.dimmer, 2, 20 } })
    tests.assert_called(timer.set_timeout_with_id, "check request dimmer values", 
        {
            get_timeout_params(2)
        })
end
