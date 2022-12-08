local args = ...

local self = hubplugins.object(table.unpack(args.creator))

self:call("_send_broadcast", args.broadcast_body)

local attempt = args.attempt + 1
if attempt < self._repeats_count then
    self:call("_create_timer", args.timer_id, args.broadcast_body, attempt)
end
