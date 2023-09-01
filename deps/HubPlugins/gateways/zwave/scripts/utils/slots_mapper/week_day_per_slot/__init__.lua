local self, item_id, dictionary_id, node_id, channel_id, device_supports_shifted_days = ...

self:_init_base("slots_mapper/base", "HUB:zwave/scripts/utils", "week_day_per_slot", item_id, dictionary_id)
self:_register("slots_mapper/week_day_per_slot", "HUB:zwave/scripts/utils")

self._node_id = node_id
self._channel_id = channel_id
self._device_supports_shifted_days = device_supports_shifted_days
