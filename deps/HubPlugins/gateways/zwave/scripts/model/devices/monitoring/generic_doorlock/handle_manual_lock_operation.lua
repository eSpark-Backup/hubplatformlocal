local self, node_id, channel_id, class_id, lock_type = ...

print("doorlook alarm 0x15; " .. lock_type .. ": manual lock by " ..
    ( lock_type == 0x01 and "cilinder or inside thumb-turn" or
      lock_type == 0x02 and "touch function" or
      lock_type == 0x03 and "inside button" or
                            "unknown reason" ) )

self:call("forward_access_control_notification_event", node_id, channel_id, class_id, "manual_lock_operation")
