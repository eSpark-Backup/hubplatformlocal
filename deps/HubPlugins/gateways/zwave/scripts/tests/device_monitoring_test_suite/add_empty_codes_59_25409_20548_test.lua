local tests = require("tests")
local zwave = require("zwave")
local core = require("core")
local events_handler = assert(loadfile("HUB:zwave/scripts/events/events_handling"))
local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local item_names = require("HUB:zwave/scripts/resources/item_names")
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

zwave.request_node_values = tests.create_mock()

local node_id = 9000
local channel_0_id = 0

local channel_0 = {
    id = channel_0_id,
    classes = {
        { id = class_ids.user_codes, supported_users_count = 30 } 
    }
}

local node = {
    id = tostring(node_id),
    manufacturer_id = 59,
    product_type = 25409,
    product_id = 20548,
    is_battery_powered = true,
    specific_type = "SPECIFIC_TYPE_SECURE_KEYPAD_DOOR_LOCK",
    generic_type = "GENERIC_TYPE_ENTRY_CONTROL",
    security = { "s0" },
    security_class_started = { "s0" },
    channels = { channel_0 }
}

zwave.add_node( node )

events_handler( {
    event = "node_added",
    node_id = node_id
} )

local user_codes_item_id = storage:call("find_item_id", item_names.user_codes, node_id, class_ids.user_codes, channel_0_id)
assert(user_codes_item_id, "Not found user codes item")

local function check_result(names)
    local codes = core.get_item(user_codes_item_id).value
    for i, name in ipairs(names) do
        tests.assert_eq(codes[tostring(i)].code, name, "check user codes matching")
    end
end

local function send_user_code(user_id, user_code)
    events_handler( {
        event = "user_code_updated",
        node_id = node_id,
        class_id = class_ids.user_codes,
        channel_id = channel_0_id,
        user_polling_status = "idle",
        user_code_ranges = {
            {
                user_id = user_id,
                user_status = "enabled",
                user_code = user_code
            }
        },
        supported_users_count = 30
    } )
end

send_user_code(1, "")
check_result({"**********"})

send_user_code(2, "1235")
check_result({"**********", "1235"})

send_user_code(3, "5421")
send_user_code(2, "")
check_result({"**********", "**********", "5421"})

send_user_code(3, "**********")
check_result({"**********", "**********", "**********"})

send_user_code(4, "**********")
check_result({"**********", "**********", "**********", "**********"})

