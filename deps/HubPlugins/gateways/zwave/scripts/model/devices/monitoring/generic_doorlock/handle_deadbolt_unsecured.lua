local self, node_id, channel_id, class_id, bolt_state = ...

print("doorlook alarm 0x2A: Deadbolt state " .. bolt_state .. " " ..
     (bolt_state == 0 and "secured" or "unsecured"))
