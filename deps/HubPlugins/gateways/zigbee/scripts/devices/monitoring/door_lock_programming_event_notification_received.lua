local handler, node, endpoint_id, cluster_name, event_source, event_name, user_id, pin_code, user_type, user_status = ...

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

if event_name == "pin_code_deleted" then
    core.remove_item_dictionary_value( pin_codes_item.id, user_id_string )
elseif event_name == "pin_code_added" or event_name == "pin_code_changed" then
    local new_mode = user_status == "occupied_enabled" and "enabled" or "disabled"
    if pin_code == "" then
        zigbee.request_user_codes(node.id, endpoint_id, user_id, user_id)
    else
        core.update_item_dictionary_value(
            pin_codes_item.id,
            user_id_string,
            handler:get_user_code_with_name(pin_code, user_id_string, pin_codes_item, new_mode)
    )
    end

end
