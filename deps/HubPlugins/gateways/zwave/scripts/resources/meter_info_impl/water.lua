local item_names = require("HUB:zwave/scripts/resources/item_names")

return  {
        cubic_meter = {
            item_name = item_names.water_meter_volume,
            ui_scale = "cubic_meter"
        },
        cubic_feet = {
            item_name = item_names.water_meter_volume,
            ui_scale = "cubic_feet"
        },
        gallons = {
            item_name = item_names.water_meter_volume,
            ui_scale = "gallons"
        },
        pulse_count = {
            item_name = item_names.water_meter_pulse,
        }
    }