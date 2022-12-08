local handler, node, endpoint_id, cluster_name, user_type, user_status, user_id, pin_code = ...

local zigbee = require("zigbee")
local core = require("core")

local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")
local storage = hubplugins.object("storage_helper", "HUB:zigbee/scripts/utils")

local pin_codes_item = handler:find_item(
    item_names.user_codes,
    node.id,
    endpoint_id,
    cluster_name,
    ""
)

assert(pin_codes_item, "user_codes item not found")

local user_id_string = tostring( math.floor(user_id) )

if user_status == "available" or user_status == "not_supported" then
    core.remove_item_dictionary_value( pin_codes_item.id, user_id_string )
else
    local new_mode = user_status == "occupied_enabled" and "enabled" or "disabled"

    core.update_item_dictionary_value(
        pin_codes_item.id,
        user_id_string,
        handler:get_user_code_with_name(pin_code, user_id_string, pin_codes_item, new_mode)
    )
end
