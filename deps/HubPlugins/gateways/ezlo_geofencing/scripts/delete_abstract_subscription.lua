-- INFO : bridge between cloud_devices lua api and ezlo geofencing plugin
-- Call: delete_abstract_subscription { "request": {}, "subscriptionRecord": {} }
local params = ...
assert(params, "Parameters are empty")

local core = assert(require "core", "Failed to load core")
local logging = assert( require "logging", "Failed to load logging")
local cloud_devices = assert(require "cloud_devices", "Failed to load cloud_devices")

local record = params.subscriptionRecord
local request = params.request

logging.debug("Deleting abstract: " .. request.abstractUUID)
for mapping_id, mapping in ipairs(record.mappings) do
    core.remove_item(mapping.itemId)
end
core.remove_device(record.deviceId)

local output_params = { abstractUUID = request.abstractUUID }
cloud_devices.remove_subscription_record(output_params)

