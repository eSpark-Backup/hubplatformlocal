local self, item_id, dictionary_id, node_id, channel_id = ...

self:_init_base("slots_mapper/base", "HUB:zwave/scripts/utils", "weekly_user_code_intervals", item_id, dictionary_id)
self:_register("slots_mapper/weekly_user_code_intervals", "HUB:zwave/scripts/utils")

self._node_id = node_id
self._channel_id = channel_id
