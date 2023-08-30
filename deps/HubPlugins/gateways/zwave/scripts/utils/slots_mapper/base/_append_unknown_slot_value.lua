local self, splitted_values, indexes_map, slot, value = ...

table.insert(splitted_values, { value = value })
table.insert(indexes_map, { slot = slot })
