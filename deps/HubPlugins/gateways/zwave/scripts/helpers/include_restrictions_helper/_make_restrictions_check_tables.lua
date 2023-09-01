local self, restrictions = ...

local categories = {}
local subcategories = {}

for _, category_info in ipairs(restrictions) do
    if category_info.subcategories then
        local subcategories_table = {}
        for _, subcategory in ipairs(category_info.subcategories) do
            subcategories_table[subcategory] = true
        end

        subcategories[category_info.category] = subcategories_table
    else
        categories[category_info.category] = true
    end
end

return categories, subcategories
