local tests = require( "tests" )

local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local events_handler = hubplugins.weak_loadfile( "HUB:zwave/scripts/events/events_handling" )
local zwave = require( "zwave" )
local core = require( "core" )
local item_names = require("HUB:zwave/scripts/resources/item_names")
local timer = require( "timer" )
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

zwave.request_node_values = tests.create_mock()

timer.cancel = tests.create_mock()
timer.set_timeout_with_id = tests.create_mock()

-- prepare and add nodes

local dimmer_item_ids = {  }
local switch_item_ids = {  }

local function is_iblinds(node_id)
    return node_id > 3
end

for node_id = 1, 6 do
    local node = {
        id = tostring(node_id),
        manufacturer_id = is_iblinds(node_id) and 647 or 1,
        product_type = 3,
        product_id = is_iblinds(node_id) and 13 or 1,
        is_battery_powered = true,

        specific_type = "",
        generic_type = "",

        security = { "none" },
        security_class_started = { "none" },

        channels = { 
            {
                id = 0,
                classes = {
                    { id = class_ids.battery },
                    { id = class_ids.switch_binary },
                    { id = class_ids.dimmer }
                }
            }
        }
    }

    zwave.add_node( node )

    events_handler( {
        event = "node_added",
        node_id = node_id 
    } )


    local dimmer_item_id = assert( storage:call("find_item_id", item_names.dimmer, node_id, class_ids.dimmer, 0 ))
    table.insert(dimmer_item_ids, dimmer_item_id)

    local switch_item_id = assert( storage:call("find_item_id", item_names.switch, node_id, class_ids.switch_binary, 0 ))
    table.insert(switch_item_ids, switch_item_id)
end




zwave.set_value_multicast = tests.create_mock()

-- do dimmer multicast

local function test_impl(params)
    hubplugins.weak_loadfile("HUB:zwave/scripts/set_item_value_multicast")({
        item_ids = params.item_ids,
        value = params.sent_value
    })

    -- check expectations

    tests.assert_called_unordered( 
        zwave.set_value_multicast, 
        "check multicast called", 
        {
            { { 1, 2, 3 }, params.class_id, 0, params.sent_value, "chip_singlecasts" },
            { { 4, 5, 6 }, params.class_id, 0, params.sent_value, "no_values_getting" },
        }
    )

    for node_id = 1, 6 do
        tests.assert_eq(core.get_item(dimmer_item_ids[node_id]).value, is_iblinds(node_id) and params.dimmer_value or 0, "Check dimmer value")
        tests.assert_eq(core.get_item(switch_item_ids[node_id]).value, is_iblinds(node_id) and params.switch_value, "Check switch value")
    end


    tests.assert_called_unordered(timer.cancel, "check timer cancelled", { { "iblinds_4_38" }, { "iblinds_5_38" }, { "iblinds_6_38" } })
    tests.assert_called_unordered(
        timer.set_timeout_with_id, 
        "check timer started", 
        { 
            { 
                120000, 
                "iblinds_4_38", 
                "HUB:zwave/scripts/timer_handlers/common/request_value",
                {
                    node_id = "4",
                    channel_id = 0,
                    class_id = class_ids.dimmer
                }
            },
            { 
                120000, 
                "iblinds_5_38", 
                "HUB:zwave/scripts/timer_handlers/common/request_value",
                {
                    node_id = "5",
                    channel_id = 0,
                    class_id = class_ids.dimmer
                }
            },
            { 
                120000, 
                "iblinds_6_38", 
                "HUB:zwave/scripts/timer_handlers/common/request_value",
                {
                    node_id = "6",
                    channel_id = 0,
                    class_id = class_ids.dimmer
                }
            } 
        })
end

tests.run_with_params(
    test_impl,
    {
        {
            description = "dimmer multicast 44",
            value = {
                item_ids = dimmer_item_ids,
                sent_value = 44,
                class_id = class_ids.dimmer,
                dimmer_value = 44,
                switch_value = true
            }
        },
        {
            description = "switch multicast true",
            value = {
                item_ids = switch_item_ids,
                sent_value = true,
                class_id = class_ids.switch_binary,
                dimmer_value = 50,
                switch_value = true
            }
        },
        {
            description = "switch multicast false",
            value = {
                item_ids = switch_item_ids,
                sent_value = false,
                class_id = class_ids.switch_binary,
                dimmer_value = 0,
                switch_value = false
            }
        }
    }
)
