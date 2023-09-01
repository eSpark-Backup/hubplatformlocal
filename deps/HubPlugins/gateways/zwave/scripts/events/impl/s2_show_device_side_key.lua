local params = ...

print("Show device side key")

local dsk_strings_from_ints = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/dsk_strings_from_ints")
local dsk = dsk_strings_from_ints( params.key )

local str = "Request DSK: ..... : "
for _, v in ipairs(dsk) do
    str = str .. math.floor(v) .. " - "
end
str = str:sub(1, -3)

print(str)

local core = assert(require("core"), "Fail to load the 'core' functionality")
core.send_ui_broadcast({ event = params.event, key = dsk })
