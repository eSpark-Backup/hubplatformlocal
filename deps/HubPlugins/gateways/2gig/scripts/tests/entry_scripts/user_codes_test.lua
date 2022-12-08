local test_utils = require("HUB:2gig/scripts/tests/test_utils")
local add_device = loadfile("HUB:2gig/scripts/add_device")
local item_factory = require("HUB:2gig/scripts/helpers/item_factory")
local item_names = require("HUB:2gig/scripts/enum/item_names")
local add_item_dict_value = loadfile("HUB:2gig/scripts/add_item_dictionary_value")
local set_item_dict_value = loadfile("HUB:2gig/scripts/set_item_dictionary_value")
local rem_item_dict_value = loadfile("HUB:2gig/scripts/remove_item_dictionary_value")
local clear_item_dict = loadfile("HUB:2gig/scripts/clear_item_dictionary")
local tests = require("tests")
local core = require("core")

add_device({
    model = "2GIG-PAD1-345",
    serial = 123
})

local pin_codes_item_id = test_utils.get_item_id(123, item_names.user_codes)

-- test add new pin code
core.generate_item_dictionary_number_id = function(...) return 1 end
add_item_dict_value({item_id = pin_codes_item_id, value = {code = "1111", name = "test1"}})

tests.assert_eq(core.get_item(pin_codes_item_id).value, {["1"] = {code = "1111", name = "test1"}}, "check core item value")

-- test add second pin code
core.generate_item_dictionary_number_id = function(...) return 2 end
add_item_dict_value({item_id = pin_codes_item_id, value = {code = "2222", name = "test2"}})

tests.assert_eq(
    core.get_item(pin_codes_item_id).value, 
    {
        ["1"] = {code = "1111", name = "test1"},
        ["2"] = {code = "2222", name = "test2"}
    }, 
    "check core item value"
)

-- test set modify pin code
core.generate_item_dictionary_number_id = nil
set_item_dict_value({item_id = pin_codes_item_id, key = "2", value = {code = "3333", name = "test3"}})

tests.assert_eq(
    core.get_item(pin_codes_item_id).value, 
    {
        ["1"] = {code = "1111", name = "test1"},
        ["2"] = {code = "3333", name = "test3"}
    }, 
    "check core item value"
)

-- test set add new pin code
set_item_dict_value({item_id = pin_codes_item_id, key = "5", value = {code = "5555", name = "test5"}})

tests.assert_eq(
    core.get_item(pin_codes_item_id).value, 
    {
        ["1"] = {code = "1111", name = "test1"},
        ["2"] = {code = "3333", name = "test3"},
        ["5"] = {code = "5555", name = "test5"}
    }, 
    "check core item value"
)

-- test remove pin code
rem_item_dict_value({item_id = pin_codes_item_id, key = "2"})

tests.assert_eq(
    core.get_item(pin_codes_item_id).value, 
    {
        ["1"] = {code = "1111", name = "test1"},
        ["5"] = {code = "5555", name = "test5"}
    }, 
    "check core item value"
)

-- test clear pin codes
clear_item_dict({item_id = pin_codes_item_id})

tests.assert_eq(
    core.get_item(pin_codes_item_id).value, 
    {}, 
    "check core item value"
)
