-- INFO : bridge between cloud_devices lua api and ezlo geofencing plugin
-- Call: create_abstract_subscription { "request": {} }
local params = ...
assert(params, "Parameters are empty")

local core = assert(require "core", "Failed to load core")
local logging = assert( require "logging", "Failed to load logging")
local cloud_devices = assert(require "cloud_devices", "Failed to load cloud_devices")
local mapping_utils = assert(require "HUB:ezlo_geofencing/scripts/helpers/mapping_utils", "Failed to load mapping_utils")

local record = {}
record["_id"] = params.request.abstractUUID
record.mappings = {}

local dev_id = mapping_utils.create_abstract_device(params.request.abstractUUID)

if dev_id then
    logging.debug("Device created: " .. dev_id)
    record.deviceId = dev_id
    record.gatewayName = "ezlo_geofencing"

    for mapping_id, mapping in ipairs(params.request.mappings) do
        local item_id = mapping_utils.create_abstract_item(dev_id, mapping)

        if item_id then
            logging.debug("Item created: " .. item_id)
            record.mappings[mapping_id] = { capability = mapping.capability,
                                            variable = mapping.variable,
                                            itemId = item_id }
        else
            logging.error("Failed to create item for capability: " .. mapping.capability .. ", variable: " .. mapping.variable)

            logging.debug("Cleaning up ...")
            for mapping_id, mapping in ipairs(record.mappings) do
                core.remove_item(mapping.itemId)
            end
            core.remove_device(dev_id)
        end
    end

    local output_params = { subscriptionRecord = record }
    cloud_devices.store_subscription_record(output_params)
else
    logging.error("Failed to create device", ...)
end

