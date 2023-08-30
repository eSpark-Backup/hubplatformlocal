local self, target_item, source_item = ...

if target_item.value_type ~= source_item.value_type then
    print(("[base_command_class.merge_items]: value types differs: %s - %s"):format(target_item.value_type, source_item.value_type))
    return
end

local find_if = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/find_in_array_if")
local append_to_table = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/append_to_table")
local add_without_duplicates = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/add_to_table_without_duplicates")

target_item.has_setter = target_item.has_setter or source_item.has_setter
target_item.has_getter = target_item.has_getter or source_item.has_getter

local source_references = source_item.info and source_item.info["zwave.references"]
local target_references = target_item.info and target_item.info["zwave.references"]

if source_references and target_references then
    for _, source_reference in ipairs(source_references) do
        local _,target_reference = find_if(target_references,function (reference)
                return reference.channel == source_reference.channel
                   and reference.class == source_reference.class
            end)
        if target_reference then
            add_without_duplicates(target_reference.managing_commands, source_reference.managing_commands)
        else
            append_to_table(target_item.info["zwave.references"], source_reference)
        end
    end
elseif source_references then
    target_item.info = target_item.info or {}
    target_item.info["zwave.references"] = source_references
end

if target_item.value_type == "token" then
    add_without_duplicates(target_item.enum, source_item.enum)
else
    print(("[base_command_class.merge_items]: how to merge items '%s' of type '%s' ?"):format(target_item.name, target_item.value_type))
end
