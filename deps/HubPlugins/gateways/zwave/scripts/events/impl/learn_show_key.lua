local params = ...

print("Learn show key")

local dsk_strings_from_ints = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/dsk_strings_from_ints")
local dsk = dsk_strings_from_ints( params.key )

local core = assert(require("core"), "Fail to load the 'core' functionality")
core.send_ui_broadcast({ event = params.event, key = dsk })
