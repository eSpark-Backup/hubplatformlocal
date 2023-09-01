local tests = require("tests")

local device_ids = require("HUB:zwave/configs/devices_list")

local broken_device_infos = {}

for _, device_id in pairs(device_ids) do
    local device_info = require("HUB:zwave/scripts/model/devices/devices_info/generated/" .. device_id)

    if device_info == nil then
        broken_device_infos[#broken_device_infos + 1] = device_id
    end
end

table.sort(broken_device_infos)

tests.assert_eq(#broken_device_infos, 0, "The following devices have broken info files: " .. hubplugins.dump(broken_device_infos))
