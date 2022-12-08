local handler, node, endpoint_id, cluster_name, event_source, event_name, user_id, pin_code = ...

local zigbee = require("zigbee")
local core = require("core")

local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")
local storage = hubplugins.object("storage_helper", "HUB:zigbee/scripts/utils")
local find_attribute = hubplugins.weak_loadfile("HUB:zigbee/scripts/helpers/find_attribute")

local function update_user_lock_operation()
    local pin_codes_item = handler:find_item(
        item_names.user_codes,
        node.id,
        endpoint_id,
        cluster_name,
        ""
    )
    if not pin_codes_item or not pin_codes_item.value[tostring(math.floor(user_id))] then return end

    local user_lock_operation_item_id = handler:find_item_id(
        item_names.user_lock_operation,
        node.id,
        endpoint_id,
        cluster_name,
        ""
    )

    if user_lock_operation_item_id then
        handler:update_value_in_core(user_lock_operation_item_id, { userId = user_id, action = event_name }) -- TODO
        handler:update_value_in_core(user_lock_operation_item_id, { userId = -1, action = "unknown" })
    end
end

if event_source == "keypad" and ( event_name == "lock" or event_name == "unlock" ) then
    update_user_lock_operation()
end

if find_attribute(node, endpoint_id, cluster_name, "LockState") ~= nil then
    zigbee.request_attribute_value(node.id, endpoint_id, cluster_name, "LockState")
end
