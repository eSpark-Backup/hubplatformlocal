local self, item_id, item_descriptor = ...

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local item_names = require("HUB:zwave/scripts/resources/item_names")

local zwave = require "zwave"
local core = require "core"


if item_descriptor.item_name == item_names.user_codes then
    zwave.set_user_code(item_descriptor.node_id, item_descriptor.channel_id, 0, "available", "")

    local item = core.get_item(item_id)

    local user_code_requests_sender = hubplugins.object("sequent_requests_sender/user_codes", "HUB:zwave/scripts/utils", item_id, item_descriptor.node_id, item_descriptor.channel_id)
    user_code_requests_sender:call("add_user_code_requests_by_item", item, true )

elseif item_descriptor.item_name == item_names.weekly_user_code_intervals
    or item_descriptor.item_name == item_names.daily_user_code_intervals then

    local slots_mapper_name = item_descriptor.tag or item_descriptor.item_name

    local item = core.get_item(item_id)

    local user_code_intervals_modifier = hubplugins.object(
                                            "sequent_requests_sender/user_code_intervals", 
                                            "HUB:zwave/scripts/utils",
                                            item_id, 
                                            item_descriptor.node_id, 
                                            item_descriptor.channel_id,
                                            slots_mapper_name)

    user_code_intervals_modifier:call("add_slots_clear_requests_by_item", item )
else
    print("clear_item_dictionary: not implemented for dictionary item " .. item_descriptor.item_name)
end