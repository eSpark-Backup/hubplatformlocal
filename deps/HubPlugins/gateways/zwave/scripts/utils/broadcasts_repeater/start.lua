local self, broadcast_body = ...

self:call("_send_broadcast", broadcast_body)

self:call("_create_timer", self:call("_generate_timer_id", broadcast_body.event), broadcast_body, 0)
