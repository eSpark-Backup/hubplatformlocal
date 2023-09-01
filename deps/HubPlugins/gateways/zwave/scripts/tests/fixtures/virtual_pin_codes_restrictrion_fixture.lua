local fixture = {}

local timer = require("timer")
local tests = require("tests")
local core = require("core")
local storage = require("storage")
local zwave = require("zwave")

local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )

function fixture.init()
    zwave.set_user_code = tests.create_mock()
    zwave.request_node_values = tests.create_mock()
    zwave.request_user_code = tests.create_mock()
end

function fixture.add_device(node_id, manufacturer_id, product_type, product_id)
    node_id = node_id or 9000
    local cc_user_code = {
        id = class_ids.user_codes,
        supported_users_count = 250,
        version = 1
    }

    local channel_0 = {
        id = 0,
        classes = { cc_user_code }
    }

    local node = {
        id = tostring(node_id),
        manufacturer_id = manufacturer_id or 0,
        product_type = product_type or 1,
        product_id = product_id or 2,
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
        node_id = node_id
    } )
end

function fixture.remove_device(node_id, manufacturer_id, product_type, product_id)
    node_id = node_id or 9000

    events_handler( {
        event = "node_removed",
        node_id = tostring(node_id),
        manufacturer_id = manufacturer_id or 0,
        product_type = product_type or 1,
        product_id = product_id or 2,
    } )
end

function fixture.find_item_id(item_name, class_id, node_id)
    node_id = node_id or 9000
    local storage_helper = hubplugins.object( "storage_helper", "HUB:zwave/scripts/utils")
    return storage_helper:call("find_item_id", item_name, node_id, class_id, 0)
end

function fixture.set_local_time(time)
    core.get_local_time = function(...) return time end
end

function fixture.check_item_value(item_id, value)
    tests.assert_eq(
        core.get_item(item_id).value,
        value,
        "check core item value"
    )
end

function fixture.check_pin_code_status_changed(params)
    local call_params = {}
    if params then
        for _, param in ipairs(params) do
            local node_id = #param == 4 and param[4] or "9000"
            table.insert(call_params, {node_id, 0, param[1], param[2], param[3]})
        end
    end

    timer.add_time((#call_params * 2000) + 2)

    tests.assert_called_unordered(zwave.set_user_code, "Check pin code status changed", call_params)
end

function fixture.check_event_table(value)
    tests.assert_eq(storage.get_table("zwave_virtual_pin_code_restriction_nearest_event_time"), value)
end

function fixture.check_timer(inited, timeout)
    if inited then
        tests.assert_eq(timer.exists("zwave_virtual_pin_codes_restrictions_timer"), true)
        tests.assert_eq(timer.get_time_to_execution("zwave_virtual_pin_codes_restrictions_timer"), timeout)
    else
        tests.assert_eq(timer.exists("zwave_virtual_pin_codes_restrictions_timer"), false)
    end
end

return fixture
