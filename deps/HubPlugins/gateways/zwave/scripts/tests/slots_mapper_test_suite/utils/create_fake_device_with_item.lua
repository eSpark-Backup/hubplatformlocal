local value_type = ...

local core = require "core"

local device_id = core.add_device({
        name = "test_device",
        category = "",
        subcategory = ""
    })


local item_id = core.add_item({
    name = "test_item",
    value_type = value_type,
    value = {},
    has_getter = true,
    has_setter = true,
    device_id = device_id,
    descriptor = {
        item_name = "test_item",
        class_id = 1,
        channel_id = 1,
        node_id = 1
    }
})

return device_id, item_id