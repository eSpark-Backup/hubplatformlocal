local params = ...

local write = io.write
write("Request DSK: .....")
local dsk = {}
for _, k in ipairs(params.key) do
    write(" - " .. math.floor(k))
    table.insert(dsk, string.format("%05d", k))
end
write('\n')

local core = assert(require("core"), "Fail to load the 'core' functionality")
core.send_ui_broadcast({ event = params.event, key = dsk })
