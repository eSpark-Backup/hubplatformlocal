local self, key = ...

local zwave = require "zwave"

zwave.request_user_code(self._node_id, self._channel_id, tonumber(key))
