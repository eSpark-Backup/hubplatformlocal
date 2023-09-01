local config = {
    name_coefficient = 0.3,
    type_coefficient = 0.1,
    category_coefficient = 0.15,
    subcategory_coefficient = 0.15,
    item_coefficient = 0.3,

    overlap_factor = 0.61
}

assert((config.name_coefficient + config.type_coefficient + config.category_coefficient + config.subcategory_coefficient + config.item_coefficient) == 1)

return config
