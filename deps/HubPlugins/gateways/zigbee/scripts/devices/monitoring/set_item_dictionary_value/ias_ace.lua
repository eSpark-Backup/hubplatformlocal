local handler, item_id, descriptor, key, value_type, value = ...

local zigbee = require("zigbee")
local core = require("core")

local node = zigbee.get_node(descriptor.node_id)
local dev_info = handler:get_device_info(node)

if not key then
    key = tostring(core.generate_item_dictionary_number_id(
        item_id,
        tonumber(dev_info.first_user_code or 1),
        tonumber(dev_info.end_user_code or 100)
    ))
end

core.update_item_dictionary_value(item_id, key, value)
