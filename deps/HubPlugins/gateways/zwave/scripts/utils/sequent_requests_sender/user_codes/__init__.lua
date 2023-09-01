local self, item_id, node_id, channel_id = ...

self:_init_base("sequent_requests_sender/base", "HUB:zwave/scripts/utils", "sequent_user_codes_requests_sender_" .. item_id)
self:_register("sequent_requests_sender/user_codes", "HUB:zwave/scripts/utils")

self._item_id = item_id
self._node_id = tonumber(node_id)
self._channel_id = channel_id
