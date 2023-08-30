local params = ...

assert(params, "Parametrs are empty")

require ("HUB:plughub/scripts/helpers/scripts_preload")

local storage = assert(require "storage", "Fail to load storage")
local item_storage_prefix = plughubplugins.weak_require("HUB:plughub/scripts/resources/item_storage_prefix")
local plughub = assert(require "plughub", "Fail to load plughub")

if storage.get_string(item_storage_prefix .. "meter_reset") == params.item_id then
    plughub.reset_consumed_energy_meter()
elseif storage.get_string(item_storage_prefix .. "switch") == params.item_id then
    plughub.set_switch_value(params.value)
else
    print("set_item_value, wrong item id " .. params.item_id)
end
