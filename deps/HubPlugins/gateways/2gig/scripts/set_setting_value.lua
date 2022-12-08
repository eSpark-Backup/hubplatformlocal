local params = ...

local twogig = require("2gig")
local core = require("core")
local storage_utils = require("HUB:2gig/scripts/helpers/storage_utils")

local setting = assert(core.get_setting(params.setting_id), "Setting not found")


local value = params.value

assert(setting.value_type ~= "action", "Action value is not implemented")
assert(type(setting.value) == type(value), "Wrong setting type '" .. type(value) .. "'" .. ", actual is '" .. type(setting.value) .. "'")

value = setting.value_min ~= nil and value < setting.value_min and setting.value_min or value
value = setting.value_max ~= nil and value > setting.value_max and setting.value_max or value

core.set_setting_value(params.setting_id, value, "synced")

local tx_id = storage_utils.find_serial(setting.device_id)
twogig.configure_device({ math.floor(tx_id) }, {
    [setting.info.tag] = value
})
