local extensions_manager = {}

function extensions_manager.get_plugin_extensions()
    return {}
end

function extensions_manager.get_plugin_extension_data(plugin_id, path, filter)
    if filter == "count" then return 0 
    else return {} end
end

return extensions_manager
