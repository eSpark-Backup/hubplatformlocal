local gateway_id, cloud_device = ...

local core = assert(require "core", "Fail to load core functionality")
local storage = assert(require "storage", "Fail to load storage functionality")

local device_info = cloud_device.info;
device_info[ "abstract" ] = cloud_device.id

local core_device_id = core.add_device({
    gateway_id = gateway_id,
    name = cloud_device.name,
    type = cloud_device.type,
    device_type_id = cloud_device.device_type_id,
    category = cloud_device.category,
    battery_powered = cloud_device.battery_powered == 1,
    info = cloud_device.info,
    subcategory = cloud_device.subcategory,
    reachable = cloud_device.reachable == 1,
    security = cloud_device.security,
});

local add_item_if_needed = loadfile("HUB:cloud_devices/scripts/helpers/add_item_if_needed")
for _, capability in ipairs(cloud_device.capabilities) do
    add_item_if_needed(core_device_id, capability);
end

local storage_keys = require("HUB:cloud_devices/scripts/resources/storage_keys")
storage.set_string(storage_keys.cloud_id_by_core_id_prefix .. core_device_id, cloud_device.id)
storage.set_string(storage_keys.core_id_by_cloud_id_prefix .. cloud_device.id, core_device_id)
