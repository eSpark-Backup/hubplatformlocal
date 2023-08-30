local self, node_id, channel_id, class_id, user_code_ranges, users_count, polling_status = ...

assert(user_code_ranges and user_code_ranges[1])
for i = 1, #user_code_ranges do
    local code = user_code_ranges[i].user_code and user_code_ranges[i].user_code:gsub("[\r\n]", "")
    if code:len() == 0 then
        user_code_ranges[i].user_code = "**********"
    end  
end

self:_super_call("schlage_doorlock", "update_user_code_value", node_id, channel_id, class_id, user_code_ranges, users_count, polling_status)
