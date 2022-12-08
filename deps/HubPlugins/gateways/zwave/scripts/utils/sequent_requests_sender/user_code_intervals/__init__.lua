local self, item_id, node_id, channel_id, slots_type = ...

self:_init_base("sequent_requests_sender/base", "HUB:zwave/scripts/utils", "sequent_user_code_intervals_sender_" .. item_id, 12000, 6000)
self:_register("sequent_requests_sender/user_code_intervals", "HUB:zwave/scripts/utils")

self._item_id = item_id
self._node_id = node_id
self._channel_id = channel_id
self._slots_type = slots_type
