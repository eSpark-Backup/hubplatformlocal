local self, inclusion_context, build_tree_context, device_node = ...

if build_tree_context.channel.id ~= 0 then
    print("Antitheft Unlock cc in " .. build_tree_context.channel.id .. " channel. How to handle it?")
    return
end

local cc = build_tree_context.command_class

local anti_theft_cc_id = require("HUB:zwave/scripts/resources/class_ids").anti_theft

local find_if = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/find_in_array_if")

local _, anti_theft_cc = find_if(build_tree_context.channel.classes, function(candidate_cc)
        return candidate_cc.id == anti_theft_cc_id
    end)

if not anti_theft_cc then
    print("No Anti-theft CC on handling Anti-theft unlock CC. How to handle it?")
    return
end

-- TODO:
