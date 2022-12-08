local tests = require("tests")
local zwave = require("zwave")
local core = require("core")
local events_handler = assert(loadfile("HUB:zwave/scripts/events/events_handling"))
local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local item_names = require("HUB:zwave/scripts/resources/item_names")
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")


zwave.request_node_values = tests.create_mock()
zwave.request_user_code = tests.create_mock()

local function test_impl(device_info)
    local node_id = 9000
    local channel_0_id = 0

    local channel_0 = {
        id = channel_0_id,
        classes = {
            { id = class_ids.user_codes, supported_users_count = 30 },
            { id = class_ids.configuration, version = 1 },
            { id = class_ids.door_lock }
        }
    }

    local node = {
        id = tostring(node_id),
        manufacturer_id = 59,
        product_type = device_info.product_type,
        product_id = device_info.product_id,
        is_battery_powered = true,

        specific_type = "SPECIFIC_TYPE_DOOR_LOCK",
        generic_type = "GENERIC_TYPE_ENTRY_CONTROL",

        security = { "none" },
        security_class_started = { "none" },

        channels = { channel_0 }
    }

    -- prepare zwave
    zwave.add_node( node )

    -- add node
    events_handler( {
        event = "node_added",
        node_id = node_id
    } )


    local function send_user_code_length(length)
        events_handler( {
            event = "configuration_value_updated",
            node_id = node_id,
            class_id = class_ids.configuration,
            channel_id = 0,
            parameter_num = 16,
            parameter_size = 1,
            value = length
        } )
    end


    local function send_user_code(user_id, code)
        events_handler( {
            event = "user_code_updated",
            node_id = node_id,
            class_id = class_ids.user_codes,
            channel_id = channel_0_id,
            user_polling_status = "idle",
            supported_users_count = 30,
            user_code_ranges = {
                {
                    user_id = user_id,
                    user_status = "enabled",
                    user_code = code
                }
            }
        } )
    end

    local user_codes_item_id = storage:call("find_item_id", item_names.user_codes, node_id, class_ids.user_codes, channel_0_id)


    zwave.request_value = tests.create_mock()

    send_user_code_length(4)
    tests.assert_called(zwave.request_value, "check request value not called for empty item value", { })
    tests.assert_eq(core.get_item(user_codes_item_id).user_code_restriction, "^\\d{4}$", "check user code regex")

    send_user_code(1, "1111")
    send_user_code(2, "2222")
    send_user_code(10, "1010")

    local user_code_requests_sender = hubplugins.object("sequent_requests_sender/user_codes", "HUB:zwave/scripts/utils", user_codes_item_id, node_id, channel_0_id)

    send_user_code_length(4)
    tests.assert_eq( user_code_requests_sender:call("get_postponed_requests_set"), nil, "check no user codes requests after report user length without changing")

    send_user_code(11, "10101") -- some user id that not required to ask
    send_user_code_length(5)
     -- tests.assert_eq({ user_code_requests_sender:call("get_requests") }, { { "2", "10" }, "1" }, "check user codes requests")
    tests.assert_eq( user_code_requests_sender:call("get_postponed_requests_set"), { ["2"] = true, ["10"] = true, ["1"] = true }, "check user codes requests")
    tests.assert_eq(core.get_item(user_codes_item_id).user_code_restriction, "^\\d{5}$", "check user code regex")

    send_user_code_length(8)
    tests.assert_eq( user_code_requests_sender:call("get_postponed_requests_set"), 
                    { ["2"] = true, ["10"] = true, ["1"] = true, ["11"] = true }, 
                    "check updated user codes requests")
    tests.assert_eq(core.get_item(user_codes_item_id).user_code_restriction, "^\\d{8}$", "check user code regex")

    user_code_requests_sender:call("remove_all_requests")

    -- remove node
    events_handler( {
        event = "node_removed",
        node_id = 9000,
        manufacturer_id = 59,
        product_type = device_info.product_type,
        product_id = device_info.product_id,
    } )

end

tests.run_with_params(
test_impl,
{
    {
        value = {
            product_type = 1,
            product_id = 0,
            user_code_restriction = "^\\d{4,10}$"
        },
        description = "unspecified schlage doorlock"
    },
    {
        value = {
            product_type = 25409,
            product_id = 20548,
            user_code_restriction = "^\\d{4,8}$"
        },
        description = "specified schlage doorlock"
    }
})
