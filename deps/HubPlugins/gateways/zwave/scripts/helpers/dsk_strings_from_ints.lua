local dsk_key_as_ints = ...

local dsk_key_as_strings = {}
for _, v in ipairs( dsk_key_as_ints ) do
    table.insert( dsk_key_as_strings, string.format( "%05d", v ) )
end

return dsk_key_as_strings
