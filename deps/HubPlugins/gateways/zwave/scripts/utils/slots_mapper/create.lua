return (function(name, item_id, dictionary_id_opt, ...)
    return hubplugins.object("slots_mapper/" .. name, "HUB:zwave/scripts/utils", item_id, dictionary_id_opt, ...) or
           hubplugins.object("slots_mapper/base", "HUB:zwave/scripts/utils", name, item_id, dictionary_id_opt)
end)(...)


