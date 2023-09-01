local self, node_id, value = ...

print("New Schlage user code length: " .. value)

assert(value >= 4 and value <= 10)

local item_names = require("HUB:zwave/scripts/resources/item_names")
local class_ids = require("HUB:zwave/scripts/resources/class_ids")

local user_codes_item_id = self:call("find_item_id", item_names.user_codes, node_id, 0, class_ids.user_codes)

if user_codes_item_id then
    local core = require("core")
    local user_codes_item = core.get_item(user_codes_item_id)

    core.update_user_code_restriction(user_codes_item_id, "^\\d{" .. math.floor(value) .. "}$")

    local user_code_requests_sender = hubplugins.object("sequent_requests_sender/user_codes", "HUB:zwave/scripts/utils", user_codes_item_id, node_id, 0)
    user_code_requests_sender:call("add_user_code_requests_by_item", user_codes_item, true, function(user_code)
        local code = user_code.code:gsub("[\r\n]", "")
        return code:len() ~= value
    end)
end
