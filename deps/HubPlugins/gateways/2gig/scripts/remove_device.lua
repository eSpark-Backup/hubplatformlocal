local params = ...

local twogig = assert(require("2gig"), "Failed to load 2gig functionality")
local core = assert(require("core"), "Failed to load core functionality")
local storage_utils = require("HUB:2gig/scripts/helpers/storage_utils")

local tx_ids = {}
local function add_device_to_remove(device_id)
    local txid = storage_utils.find_serial(device_id)
    table.insert(tx_ids, math.floor(txid))
end

local device = core.get_device(params.deviceId)
if device ~= nil then
    add_device_to_remove(device.id)
end

local devices = core.get_devices()
if devices ~= nil then
    for _, device in ipairs(devices) do
        if device.parent_device_id == params.deviceId then
            add_device_to_remove(device.id)
        end
    end
end

while device.parent_device_id ~= "" do
    device = core.get_device(device.parent_device_id)
    add_device_to_remove(device.id)
end

if next(tx_ids) ~= nil then
    twogig.remove_device(tx_ids)
end
