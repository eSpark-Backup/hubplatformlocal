local params = ...
-- params = {
--     id = <cloud_device_id>
-- }

local storage = assert(require "storage", "Fail to load storage functionality")

local storage_keys = require("HUB:cloud_devices/scripts/resources/storage_keys")
local core_device_id = assert(storage.get_string(storage_keys.core_id_by_cloud_id_prefix .. params.id), "Expecting device to be indexed in storage")

loadfile("HUB:cloud_devices/scripts/helpers/remove_device")(core_device_id, params.id)
