local value = {
    i = 0,
    j = 0,
    factor = 0,
    sorted_index = 0
}

local function new_matrix_value()
    return setmetatable({}, { __index = value } )
end

return setmetatable({},{
    __index = function(t,k)
        t[k] = setmetatable({},{
            __index = function(t,k)
                t[k] = new_matrix_value()
                return t[k]
            end
        })
        return t[k]
    end
})
