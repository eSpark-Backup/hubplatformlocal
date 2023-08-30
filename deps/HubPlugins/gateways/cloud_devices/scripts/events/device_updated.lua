local params = ...
-- params = {
--     id = <cloud_device_id>
--     <changedField> = <new_value>
-- }

local core = assert(require "core", "Fail to load core functionality")
local storage = assert(require "storage", "Fail to load storage functionality")

local storage_keys = require("HUB:cloud_devices/scripts/resources/storage_keys")
local core_device_id = assert(storage.get_string(storage_keys.core_id_by_cloud_id_prefix .. params.id), "Expecting device to be indexed in storage")

if params.reachable then
    core.update_reachable_state(core_device_id, params.reachable == 1)
end

if params.capabilities then
    loadfile("HUB:cloud_devices/scripts/helpers/update_items_if_needed")(core_device_id, params.capabilities)
end
