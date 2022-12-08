local self, node_id, channel_id, event_type, user_id = ...

local core = require("core")
local zwave = require("zwave")
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local item_names = require("HUB:zwave/scripts/resources/item_names")

local item_id = storage:call("find_item_id", item_names.user_codes, node_id, class_ids.user_codes, channel_id)
assert(item_id, "Can not find user codes item")

local user_code_requests_sender = hubplugins.object("sequent_requests_sender/user_codes", "HUB:zwave/scripts/utils", item_id, node_id, channel_id)

if user_id and user_id > 0 then
    user_code_requests_sender:call("add_requests", { tostring(math.floor(user_id)) })

elseif event_type == "new_user_code_added" then
    zwave.request_user_code(node_id, channel_id)
    user_code_requests_sender:call("remove_all_requests") -- ??

elseif event_type == "all_user_codes_deleted" then
    user_code_requests_sender:call("remove_all_requests")
    core.update_item_value(item_id, {})
    storage:call("delete_table", storage:call("get_usernames_tablename", item_id))

    self:call("clear_all_user_codes_intervals", node_id, channel_id)

elseif event_type == "single_user_code_deleted" then
    local item = core.get_item(item_id)
    user_code_requests_sender:call("add_user_code_requests_by_item", item)
end
