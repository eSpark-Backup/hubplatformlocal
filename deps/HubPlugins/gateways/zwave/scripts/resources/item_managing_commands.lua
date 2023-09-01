local item, command_class = ...

local item_managing_commands_impl = hubplugins.weak_loadfile( "HUB:zwave/scripts/resources/item_managing_commands_impl/"..tostring(command_class.id))

if item_managing_commands_impl then
    return item_managing_commands_impl(item)
end
