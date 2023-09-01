local self, node_id, channel_id, class_id, access_type = ...

print(("doorlook alarm 0xA1; manually enter '%s' access code exceeds code limit")
        :format((access_type == 0x01 and "user") or (access_type == 0x03 and "master")))
