local self, name, item_id, dictionary_id = ...

self:_init_base("slots_mapper/base", "HUB:zwave/scripts/utils", name, item_id, dictionary_id)
self:_register("slots_mapper_with_command_run", "HUB:zwave/scripts/tests/slots_mapper_test_suite")
