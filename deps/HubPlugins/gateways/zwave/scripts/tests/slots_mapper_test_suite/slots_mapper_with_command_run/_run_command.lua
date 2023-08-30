local self, slot, value = ...

local tests = require "tests"
local storage = require "storage"

local table_name = "saved_command." .. self._item_id .. "." .. tostring(self._dictionary_id)

tests.assert_eq(storage.get_table(table_name), nil, "check no saved command")

storage.set_table( table_name, {
    slot = slot,
    value = value
})
