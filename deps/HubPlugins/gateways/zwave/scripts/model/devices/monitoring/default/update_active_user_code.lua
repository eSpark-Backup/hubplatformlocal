local self, item_id, user_id, code, mode = ...

user_id = tostring( math.floor( user_id ) )

local core = require "core"
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

local username = storage:call("get_table_value", storage:call("get_usernames_tablename", item_id), user_id )
if not username then
    username = "Code " .. user_id
    storage:call("set_table_value", storage:call("get_usernames_tablename", item_id), user_id, username )
end
assert(username, "Cannot find username for id " .. user_id .. " in storage")

local user_code = {
    code = code, 
    name = username,
    mode = mode
}

core.update_item_dictionary_value(item_id, user_id, user_code)
