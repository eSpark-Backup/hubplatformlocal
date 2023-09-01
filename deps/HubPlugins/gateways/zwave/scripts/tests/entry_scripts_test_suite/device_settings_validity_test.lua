local tests = require("tests")

local device_ids = require("HUB:zwave/configs/devices_list")

local broken_device_settings = {}

for _, device_id in pairs(device_ids) do
    local device_info = require("HUB:zwave/scripts/model/devices/devices_info/generated/" .. device_id)
    assert(device_info ~= nil)

    if device_info.settings_count ~= nil then
        for idx = 1, device_info.settings_count do
            local setting_info = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_setting_info")(device_id, idx)
            if setting_info == nil then
                broken_device_settings[#broken_device_settings + 1] = device_id .. "/" .. idx
            end
        end
    end
end

table.sort(broken_device_settings)

tests.assert_eq(#broken_device_settings, 0, "The following setting info files are broken: " .. hubplugins.dump(broken_device_settings))
