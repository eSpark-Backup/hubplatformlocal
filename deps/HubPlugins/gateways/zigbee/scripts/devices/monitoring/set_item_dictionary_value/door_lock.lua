local handler, item_id, descriptor, key, value_type, value = ...

local zigbee = require("zigbee")
local core = require("core")
local storage = hubplugins.object("storage_helper", "HUB:zigbee/scripts/utils")

local item = core.get_item(item_id)

if not key then
    key = tostring(math.floor(core.generate_item_dictionary_number_id(
        item_id,
        1,
        item.elements_max_number
    )))
end

storage:call("set_table_value", storage:call("get_usernames_tablename", item_id), key, value.name)

zigbee.send_command(
    descriptor.node_id,
    descriptor.endpoint_id,
    descriptor.cluster_name,
    "SetPinCode",
    {
        user_id = tonumber(key),
        pin_code = value.code,
        user_status = value.mode == "disabled" and "occupied_disabled" or "occupied_enabled"}
)
