local water_valve = assert(require "water_valve", "Fail to load water_valve")

return water_valve.should_invert_control and water_valve.should_invert_control() or false
