local self, user_id = ...

return hubplugins.weak_loadfile("HUB:zwave/scripts/utils/slots_mapper/create")(
                                self._slots_type,
                                self._item_id,
                                math.floor(tonumber(user_id)),
                                self._node_id,
                                self._channel_id)
