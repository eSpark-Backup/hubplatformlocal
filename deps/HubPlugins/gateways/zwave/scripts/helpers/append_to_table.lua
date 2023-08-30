local function append_to_table(target_table, ...--[[values]])
    assert(type(target_table) == 'table', "'target_table' must be a table")

    for _, value in ipairs(table.pack(...)) do
        target_table[#target_table + 1] = value
    end
end

append_to_table(...--[[script args]])
