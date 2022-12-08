local cloud_devices = assert(require "cloud_devices", "Fail to load cloud devices functionality")
local core = assert(require "core", "Fail to load core functionality")

local storage = assert(require "storage", "Fail to load storage functionality")
local storage_keys = require("HUB:cloud_devices/scripts/resources/storage_keys")

cloud_devices.subscribe("HUB:cloud_devices/scripts/events/events_handling")

local plugin_gateway_id = core.get_gateway().id
local core_devices = core.get_devices()

local core_devices_set = {}
if core_devices then
    local update_items_if_needed = loadfile("HUB:cloud_devices/scripts/helpers/update_items_if_needed")
    local remove_device = loadfile("HUB:cloud_devices/scripts/helpers/remove_device")
    for _, core_device in ipairs(core_devices) do
        if core_device.gateway_id == plugin_gateway_id then
            core_devices_set[core_device.id] = true

            local cloud_device_id = storage.get_string(storage_keys.cloud_id_by_core_id_prefix .. core_device.id)
            local cloud_device
            if cloud_device_id then
                cloud_device = cloud_devices.get_device(cloud_device_id)
                if cloud_device then
                    if core_device.reachable ~= (cloud_device.reachable == 1) then
                        print("update device " .. core_device.id .. " reachable state to " .. tostring(cloud_device.reachable == 1))
                        core.update_reachable_state(core_device.id, cloud_device.reachable == 1)
                    end
                    update_items_if_needed(core_device.id, cloud_device.capabilities)
                end
            end
            if not cloud_device_id or not cloud_device then
                print("removing device " .. core_device.id .. ", no corresponding cloud device found")
                remove_device(core_device.id)
            end
        end
    end
end

local add_device = loadfile("HUB:cloud_devices/scripts/helpers/add_device")
local cloud_devices_ids = cloud_devices.get_all_devices_ids()
for _, cloud_device_id in ipairs(cloud_devices_ids) do
    local core_device_id = storage.get_string(storage_keys.core_id_by_cloud_id_prefix .. cloud_device_id)
    if not core_device_id or not core_devices_set[core_device_id] then
        local cloud_device = cloud_devices.get_device(cloud_device_id)
        add_device(plugin_gateway_id, cloud_device)
    end
end
