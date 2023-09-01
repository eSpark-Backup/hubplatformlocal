local self, storage_table_prefix, item_id, dictionary_id = ...

assert(item_id)

self:_register("slots_mapper/base", "HUB:zwave/scripts/utils")

self._storage_table_prefix = storage_table_prefix
self._item_id = item_id
self._dictionary_id = tostring(dictionary_id)
