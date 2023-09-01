local mfg_specific_tag = ".mfg_specific"

return {
    
    add_mfg_specific_tag = function(tag) 
        return tag == nil and mfg_specific_tag or 
               string.find(tag, mfg_specific_tag) and tag or
               tag .. mfg_specific_tag
    end,

    remove_mfg_specific_tag = function(tag) 
        local result = string.gsub(tag or "", mfg_specific_tag, "")
        return result ~= "" and result or nil
    end,
}
