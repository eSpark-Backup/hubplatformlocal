local security_tag = ".security_tag"

return {
    
    add_security_tag = function(tag) 
        return tag == nil and security_tag or 
               string.find(tag, security_tag) and tag or
               tag .. security_tag
    end,

    remove_security_tag = function(tag) 
        local result = string.gsub(tag or "", security_tag, "")
        return result ~= "" and result or nil
    end,
}
