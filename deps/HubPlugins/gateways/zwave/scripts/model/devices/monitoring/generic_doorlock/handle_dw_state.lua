local self, node_id, channel_id, class_id, dw_state = ...

print("doorlook alarm 0x2B: DW state " .. dw_state .. " " ..
     (dw_state == 0 and "opened" or "closed"))
