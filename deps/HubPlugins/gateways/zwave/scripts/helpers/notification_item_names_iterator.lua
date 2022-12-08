-- arg of script is single name or array of names
local names = ...

if type(names) == "string" then
    names = { names }
end

return ipairs(names)
