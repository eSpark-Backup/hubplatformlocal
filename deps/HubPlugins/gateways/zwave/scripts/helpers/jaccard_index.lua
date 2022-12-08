-- Visit Wikipedia page about Jaccard index:
-- https://en.wikipedia.org/wiki/Jaccard_index

local set1, set2 = ...

local table_length = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/table_length")

local a = table_length(set1)
local b = table_length(set2)
local c = 0

for key in pairs(set1) do
    if set2[key] then
        c = c + 1
    end
end

return c / (a + b - c)
