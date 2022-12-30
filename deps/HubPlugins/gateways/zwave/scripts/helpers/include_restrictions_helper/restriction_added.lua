local self, restrictions = ...

local function check_format()
    if type(restrictions) ~= "table" then return false end

    local restrictions_counter = 0
    for idx, value in pairs(restrictions) do
        restrictions_counter = restrictions_counter + 1
        if idx ~= restrictions_counter then
            print("Restrictions list is not an array")
            return false
        end

        if not value.category or type(value.category) ~= "string" then
            print("Incorrect restriction category at position " .. idx)
            return false
        end

        if value.subcategories then
            if type(value.subcategories) ~= "table" then return false end
            local subcategories_counter = 0
            for s_idx, subcategory in pairs(value.subcategories) do
                subcategories_counter = subcategories_counter + 1
                if s_idx ~= subcategories_counter then
                    print("Subcategories list is not an array at position " .. idx)
                    return false
                end

                if type(subcategory) ~= "string" then
                    print("Incorrect restriction subcategory at position " .. idx)
                    return false
                end
            end
        end
    end

    return true
end



if check_format() then
    self:call("storage/set_inclusion_restrictions", restrictions)
end
