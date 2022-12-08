local function deep_equal_impl( a, b )
    if a == b then
        return true
    elseif type( a ) == "table" and type( b ) == "table" then
        for key, value in pairs( a ) do
            if not deep_equal_impl( value, b[ key ] ) then
                return false
            end
        end

        for key, value in pairs( b ) do
            if a[ key ] == nil then
                return false
            end
        end

        return true
    else
        return false
    end
end

return deep_equal_impl( ... )