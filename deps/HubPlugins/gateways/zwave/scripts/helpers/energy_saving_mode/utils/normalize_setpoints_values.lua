local self, values, device_id = ...

local needed_spread = self:call("utils/get_setpoints_spread", device_id)

local real_spread = values.cool - values.heat

if real_spread >= needed_spread then return values end

local needed_correction = (needed_spread - real_spread) / 2

return {
    heat = math.ceil(values.heat - needed_correction),
    cool = math.ceil(values.cool + needed_correction)
}
