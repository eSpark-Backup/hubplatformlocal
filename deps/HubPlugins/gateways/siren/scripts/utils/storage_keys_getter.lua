local function format_key(prefix, item_name, index)
    local result = prefix .. "_" .. item_name
    if index ~= nil then
        result = result .. "_" .. index
    end
    return result
end

return {
    get_item_key = function(item_name, index)
        return format_key("siren_item", item_name, index)
    end,
    get_device_key = function(item_name, index)
        return format_key("siren_device", item_name, index)
    end,
    get_speaker_command_id = function()
        return "speaker_command_id"
    end
}
