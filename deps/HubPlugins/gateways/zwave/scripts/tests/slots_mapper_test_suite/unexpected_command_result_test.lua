local tests = require "tests"

local _, item_id = loadfile("HUB:zwave/scripts/tests/slots_mapper_test_suite/utils/create_fake_device_with_item")("dictionary.array.int")

local function get_slots_mapper()
    return loadfile("HUB:zwave/scripts/utils/slots_mapper/create")("my_name", item_id, "dict_id")
end

do
    local commands = get_slots_mapper():call("set_values", { 1, 5, 10 })
    tests.assert_eq(#commands, 3)
    for _, command in ipairs(commands) do
        get_slots_mapper():call("on_slot_value_changed", command.slot, command.value ~= 5 and command.value or nil)
    end

    tests.assert_eq(get_slots_mapper():call("get_values"), { 1, 10 }, "check values after unexpected nil value instead of added")
end

do
    local commands = get_slots_mapper():call("set_values", { 9, 10 })
    tests.assert_eq(#commands, 1)
    get_slots_mapper():call("on_slot_value_changed", commands[1].slot, nil)

    tests.assert_eq(get_slots_mapper():call("get_values"), { 10 }, "check values after unexpected nil value instead of replaced")
end

do
    local commands = get_slots_mapper():call("set_values", { })
    tests.assert_eq(#commands, 1)
    get_slots_mapper():call("on_slot_value_changed", commands[1].slot, 2)

    tests.assert_eq(get_slots_mapper():call("get_values"), { 2 }, "check values after unexpected value instead of removed")
end

do
    local commands = get_slots_mapper():call("set_values", { 1, 2, 3 })
    tests.assert_eq(#commands, 2)
    get_slots_mapper():call("on_slot_value_changed", commands[1].slot, commands[1].value)
    get_slots_mapper():call("on_slot_value_changed", 10, 100)
    get_slots_mapper():call("on_slot_value_changed", commands[2].slot, commands[2].value)

    tests.assert_eq(get_slots_mapper():call("get_values"), { 1, 2, 3, 100 }, "check values after unexpected value instead of removed")
end

do
    local commands = get_slots_mapper():call("set_values", { 11, 4, 100, 12, 2 })

    for _, command in ipairs(commands) do
        get_slots_mapper():call("on_slot_value_changed", command.slot, nil)
    end

    tests.assert_eq(get_slots_mapper():call("get_values"), { 100, 2 }, "check values after several unexpected nils instead of replaced")
end
