local self, node_id, channel_id, class_id, user_code_ranges, users_count, polling_status = ...

assert(user_code_ranges and user_code_ranges[1])

local core = require("core")
local zwave = require("zwave")
local item_names = require("HUB:zwave/scripts/resources/item_names")

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local item_id = storage:call("find_item_id", item_names.user_codes, node_id, class_id, channel_id)

local user_code_requests_sender = hubplugins.object("sequent_requests_sender/user_codes", "HUB:zwave/scripts/utils", item_id, node_id, channel_id)

local max_user_id = 1
local main_user_id = user_code_ranges[1].user_id

local reported_user_ids = {}


for _, range in ipairs(user_code_ranges) do
    local first_user_id = math.max(range.user_id, 1)
    local last_user_id = range.user_id == 0 and users_count or 
                        range.last_user_id and range.last_user_id > first_user_id and range.last_user_id or 
                        first_user_id
    max_user_id = math.max(last_user_id, max_user_id)

    local status = range.user_status 
    local user_code_exists = status ~= "available" and status ~= "not_available"

    if (first_user_id == 1 and last_user_id == users_count or range.user_id == 0) and not user_code_exists then
        user_code_requests_sender:call("remove_all_requests")
        core.update_item_value(item_id, {})

        storage:call("delete_table", storage:call("get_usernames_tablename", item_id))

        self:call("clear_all_user_codes_intervals", node_id, channel_id)
    else
        -- TODO: optimize. This cycle requires optimization for wide ranges from user code cc V2
        local code = range.user_code and range.user_code:gsub("[\r\n]", "")

        for user_id = first_user_id, last_user_id do
            local user_id_str = tostring(math.floor(user_id))

            if user_code_exists then
                if code and code:len() > 0 then
                    self:call("update_active_user_code", item_id, user_id, code, status)
                else
                    print("user code is empty but status is " .. status)
                end  

            elseif core.get_item_dictionary_value(item_id, user_id_str) then
                print("User code " .. user_id_str .. " removed")
                core.remove_item_dictionary_value(item_id, user_id_str)

                storage:call("remove_table_value", storage:call("get_usernames_tablename", item_id), user_id_str)

                self:call("clear_user_code_intervals", node_id, channel_id, user_id)
            end

            if polling_status ~= "polling" then
                table.insert(reported_user_ids, user_id_str)
            end
        end
    end
end

if main_user_id ~= 0 then
    if polling_status == "polling"  then
        if users_count and users_count > 0 then
            local polling_status_item_id = self:call("find_item_id", item_names.user_codes_scan_progress, node_id, channel_id, class_id)
            local new_value = math.floor((max_user_id / (users_count / 100)) + 0.5)
            core.update_item_value(polling_status_item_id, new_value)
        end

        user_code_requests_sender:call(
            "remove_requests_if", 
            function(request_id) 
                return tonumber(request_id) >= main_user_id 
            end)
    else
        user_code_requests_sender:call("on_report", reported_user_ids, true)
    end
end
