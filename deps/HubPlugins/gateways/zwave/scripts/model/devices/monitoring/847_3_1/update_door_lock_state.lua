local self, node_id, channel_id, door_mode, door_condition = ...

door_mode = door_mode == "secured" and "unsecured" or
        door_mode == "unsecured" and "secured" or
        door_mode

self:_super_call("generic_doorlock", "update_door_lock_state", node_id, channel_id, door_mode, door_condition)
