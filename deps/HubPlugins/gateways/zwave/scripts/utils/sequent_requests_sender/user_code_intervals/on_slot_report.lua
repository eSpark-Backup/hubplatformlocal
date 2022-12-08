local self, user_id, slot_id, value = ...

local slots_mapper = self:call("get_slots_mapper", user_id)

local next_command = slots_mapper:call("on_slot_value_changed", slot_id, value)

self:call(next_command and "on_intermediate_report" or "on_report", tostring(math.floor(user_id)))
