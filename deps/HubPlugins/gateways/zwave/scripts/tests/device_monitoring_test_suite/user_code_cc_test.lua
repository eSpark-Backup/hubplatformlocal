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
        { id = class_ids.user_codes, supported_users_count = 250 },
    }
}

local node = {
    id = tostring(node_id),
    manufacturer_id = 0,
    product_type = 1,
    product_id = 2,
    is_battery_powered = true,

    specific_type = "",
    generic_type = "",

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

local function send_user_code(user_status_id, user_id, supported_users_count, user_code)
    events_handler( {
        event = "user_code_updated",
        node_id = node_id,
        class_id = class_ids.user_codes,
        channel_id = channel_0_id,
        user_polling_status = "idle",
        user_code_ranges = {
            {
                user_id = user_id,
                user_status = user_status_id,
                user_code = user_code or "1234"
            }
        },
        supported_users_count = supported_users_count or 250
    } )
end

local user_codes_item_id = storage:call("find_item_id", item_names.user_codes, node_id, class_ids.user_codes, channel_0_id)


core.update_item_dictionary_value(user_codes_item_id, "10", { code = "2222", name = "John" })

do
    core.update_item_dictionary_value(user_codes_item_id, "3", { code = "1111", name = "Richard" })
    send_user_code("available", 3)
    tests.assert_eq(core.get_item(user_codes_item_id).value, { ["10"] = { code = "2222", name = "John" } })
end

do
    core.update_item_dictionary_value(user_codes_item_id, "3", { code = "1111", name = "Richard" })
    send_user_code("not_available", 3)
    tests.assert_eq(core.get_item(user_codes_item_id).value, { ["10"] = { code = "2222", name = "John" } })
end

do
    send_user_code("enabled", 3)

    tests.assert_eq(core.get_item(user_codes_item_id).value, { 
            ["10"] = { code = "2222", name = "John" },
            ["3"] = { code = "1234", name = "Code 3", mode = "enabled" },
        })
end

do
    send_user_code("disabled", 3, 250, "0234\r\n")

    tests.assert_eq(core.get_item(user_codes_item_id).value, { 
            ["10"] = { code = "2222", name = "John" },
            ["3"] = { code = "0234", name = "Code 3", mode = "disabled" },
        })
end

do
    send_user_code("available", 0)

    tests.assert_eq(core.get_item(user_codes_item_id).value, { })
end

do
    send_user_code("enabled", 0, 1)

    tests.assert_eq(core.get_item(user_codes_item_id).value, { ["1"] = { code = "1234", name = "Code 1", mode = "enabled" } })
end
