-- Visit Wikipedia page about Jaro distance algorithm:
-- https://en.wikipedia.org/wiki/Jaro%E2%80%93Winkler_distance#Jaro_Similarity

local a, b = ...

assert((type(a) == "string") and (type(b) == "string"), "args of algorithm must be strings")

local a_length = string.len(a)
local b_length = string.len(b)

if a_length == 0 then
    return b_length == 0 and 1 or 0
end
if b_length == 0 then
    return 0
end

local match_distance = math.floor(math.max(a_length, b_length) / 2 - 1)

local a_match = {}
local b_match = {}

local matching_characters = 0
for a_index = 1, a_length do
    local min_index = math.max(a_index - match_distance, 1)
    local max_index = math.min(a_index + match_distance, b_length)

    for b_index = min_index, max_index do
        if not b_match[b_index] and (a:sub(a_index, a_index) == b:sub(b_index, b_index)) then
            a_match[a_index] = true
            b_match[b_index] = true
            matching_characters = matching_characters + 1
            break
        end
    end
end

if matching_characters == 0 then
    return 0
end

local transpositions = 0
local b_index = 1
for a_index = 1, a_length do
    if a_match[a_index] then
        while not b_match[b_index] do
            b_index = b_index + 1
        end

        if a:sub(a_index, a_index) ~= b:sub(b_index, b_index) then
            transpositions = transpositions + 1
        end

        b_index = b_index + 1
    end
end

return  (
            (matching_characters / a_length)
        +   (matching_characters / b_length)
        +   ((matching_characters - math.floor(transpositions / 2)) / matching_characters)
        )
        / 3
