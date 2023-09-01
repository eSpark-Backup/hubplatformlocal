local cc_ids = require("HUB:zwave/scripts/resources/class_ids")

return {
    class_id = cc_ids.sensor_binary,
    subcategory_to_sensor_type = {
        door = 0x0A, 
        leak = 0x06,
        motion = 0x0C,
        smoke = 0x02,
        co = 0x03,
        glass = 0x0D,
        co2 = 0x04,
        tilt = 0x0B,
        heat = 0x05,
        freeze = 0x07
    }
}
