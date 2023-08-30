local cc_ids = require("HUB:zwave/scripts/resources/class_ids")

return {
    class_id = cc_ids.alarm_sensor,
    subcategory_to_sensor_type = {
        smoke = "smoke_alarm",
        co = "co_alarm",
        co2 = "co2_alarm",
        heat = "heat_alarm",
        leak = "water_alarm"
    }
}
