local params = ...

local args = {
    deviceAddress = params.deviceAddress,
    uuid = "0000ffe9-0000-1000-8000-00805f9b34fb",
    data = { 0x56, params.value.r, params.value.g, params.value.b, 0x00, 0xf0, 0xaa }
}

require("HUB:bluetooth/scripts/write_characteristic", args)