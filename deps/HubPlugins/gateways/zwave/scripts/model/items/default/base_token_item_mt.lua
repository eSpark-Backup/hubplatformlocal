local base = {
    value_type = "token",
    value = "unknown",
    has_getter = true,
    has_setter = false
}

return {
    __index = function(t, k)
        if base[k] ~= nil then
            return base[k]
        elseif k == "enum" then
            t.enum = { base.value }
            return t.enum
        end
    end
}
