local core_device_id, cloud_device_id = ...

local core = assert(require "core", "Fail to load core functionality")
local storage = assert(require "storage", "Fail to load storage functionality")

core.remove_device(core_device_id)

local storage_keys = require("HUB:cloud_devices/scripts/resources/storage_keys")
cloud_device_id = cloud_device_id or storage.get_string(storage_keys.cloud_id_by_core_id_prefix .. core_device_id)
if cloud_device_id then
    storage.delete(storage_keys.core_id_by_cloud_id_prefix .. cloud_device_id)
end
storage.delete(storage_keys.cloud_id_by_core_id_prefix .. core_device_id)
