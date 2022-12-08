local self, item_id, dictionary_id = ...

self:_init_base("slots_mapper/base", "HUB:zwave/scripts/utils", "mergable", item_id, dictionary_id)
self:_register("slots_mapper_with_values_merge", "HUB:zwave/scripts/tests/slots_mapper_test_suite")
