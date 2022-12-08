local config = {
    name_coefficient = 0.2,
    type_coefficient = 0.04,
    category_coefficient = 0.13,
    subcategory_coefficient = 0.13,
    item_coefficient = 0.5,

    overlap_factor = 0.5
}

assert((config.name_coefficient + config.type_coefficient + config.category_coefficient + config.subcategory_coefficient + config.item_coefficient) == 1)

return config
