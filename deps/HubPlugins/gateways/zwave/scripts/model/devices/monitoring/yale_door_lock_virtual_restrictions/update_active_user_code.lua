local self, item_id, user_id, code, mode = ...

local core = require("core")

local item = assert(core.get_item(item_id))

local user_id_string = tostring( math.floor( user_id ) )
local user_code_info = item.value and item.value[user_id_string]
if user_code_info and user_code_info.code == code then
    mode = user_code_info.mode
end

self:_super_call("generic_doorlock", "update_active_user_code", item_id, user_id, code, mode)
