local tests = require "tests"

local create_linked_table = loadfile("HUB:zwave/scripts/helpers/create_linked_table")

local linked_table = create_linked_table()

linked_table["w"] = 0
linked_table["x"] = false
linked_table["y"] = true
linked_table["z"] = ""

local function check_table(expectations)
    local index = 0
    for key, value in pairs(linked_table) do
        index = index + 1
        tests.assert_eq(key, expectations[index][1], "check key")
        tests.assert_eq(value, expectations[index][2], "check value")
    end

    tests.assert_eq(index, #expectations, "check size")

    for _, key_to_value in ipairs(expectations) do
        tests.assert_eq(linked_table[key_to_value[1]], key_to_value[2], "check getting value by key " .. tostring(key_to_value[1]))
    end
end


check_table(
{ 
    { "w", 0 },
    { "x", false },
    { "y", true },
    { "z", "" }
})

linked_table["w"] = 0

check_table(
{ 
    { "x", false },
    { "y", true },
    { "z", "" },
    { "w", 0 }
})

linked_table["w"] = nil
linked_table["x"] = nil
linked_table["w"] = nil -- reset nil

check_table(
{ 
    { "y", true },
    { "z", "" }
})

linked_table[1] = 1
linked_table[true] = 2
linked_table[false] = 3 
linked_table[2] = 4

check_table(
{ 
    { "y", true },
    { "z", "" },
    { 1, 1 },
    { true, 2 },
    { false, 3 },
    { 2, 4 }
})

-- performance test
if false then
    linked_table = create_linked_table()
    for i = 1, 1000000 do
        linked_table[i] = i
    end

    for i = 1, 1000000 do
        linked_table[i] = linked_table[i + 1]
    end

    for i = 1, 1000000 do
        linked_table[i] = nil
    end
end