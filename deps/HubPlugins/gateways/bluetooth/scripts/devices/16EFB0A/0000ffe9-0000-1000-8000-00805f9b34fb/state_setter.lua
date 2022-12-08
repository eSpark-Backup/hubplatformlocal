local params = ...

local data
if params.value then
    data = { 0xcc, 0x23, 0x33 }
else
    data = { 0xcc, 0x24, 0x33 }
end

local args = {
    deviceAddress = params.deviceAddress,
    uuid = "0000ffe9-0000-1000-8000-00805f9b34fb",
    data = data
}

require("HUB:bluetooth/scripts/write_characteristic", args)
