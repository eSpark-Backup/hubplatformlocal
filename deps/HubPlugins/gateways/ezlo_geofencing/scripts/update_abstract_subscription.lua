-- INFO : bridge between cloud_devices lua api and ezlo geofencing plugin
-- Call: update_abstract_subscription { "request": {}, "subscriptionRecord": {}, "mappingOperations": {} }
local params = ...
assert(params, "Parameters are empty")

local core = assert(require "core", "Failed to load core")
local logging = assert( require "logging", "Failed to load logging")
local cloud_devices = assert(require "cloud_devices", "Failed to load cloud_devices")
local mapping_utils = assert(require "HUB:ezlo_geofencing/scripts/helpers/mapping_utils", "Failed to load mapping_utils")

function find_mapping(mappings, capability, variable)
    for mapping_id, mapping in ipairs(mappings) do
        if mapping.capability == capability and mapping.variable == variable then
            return mapping
        end
    end
    return nil
end

local record = params.subscriptionRecord
local request = params.request

local mappings_to_add = params.mappingOperations.add
local mappings_to_update = params.mappingOperations.update
local mappings_to_remove = params.mappingOperations.remove
local mappings_to_keep = params.mappingOperations.keep

local device_id = record.deviceId

local new_record = {}
new_record["_id"] = record["_id"]
new_record.deviceId = record.deviceId
new_record.gatewayName = record.gatewayName
new_record.mappings = {}

local mapping_index = 1

if mappings_to_add then
    for _, mapping_key in ipairs(mappings_to_add) do
        local mapping = find_mapping(request.mappings, mapping_key.capability, mapping_key.variable)
        local item_id = mapping_utils.create_abstract_item(device_id, mapping)

        if item_id then
            logging.debug("Item created: " .. item_id)
            new_record.mappings[mapping_index] = { capability = mapping.capability,
                                                variable = mapping.variable,
                                                itemId = item_id }
            mapping_index = mapping_index + 1
        else
            logging.error("Failed to create item for capability: " .. mapping.capability .. ", variable: " .. mapping.variable)

            logging.debug("Cleaning up new items...")
            for _, mapping in ipairs(new_record.mappings) do
                core.remove_item(mapping.itemId)
            end

            return
        end
    end
end

if mappings_to_update then
    for _, mapping_key in ipairs(mappings_to_update) do
        local request_mapping = find_mapping(request.mappings, mapping_key.capability, mapping_key.variable)
        local database_mapping = find_mapping(record.mappings, mapping_key.capability, mapping_key.variable)

        local item = core.get_item(database_mapping.itemId)
        mapping_utils.update_abstract_item(item, request_mapping)

        new_record.mappings[mapping_index] = database_mapping
        mapping_index = mapping_index + 1
    end
end

if mappings_to_keep then
    for _, mapping_key in ipairs(mappings_to_keep) do
        local mapping = find_mapping(record.mappings, mapping_key.capability, mapping_key.variable)
        new_record.mappings[mapping_index] = mapping
        mapping_index = mapping_index + 1
    end
end

if mappings_to_remove then
    for _, mapping_key in ipairs(mappings_to_remove) do
        local mapping = find_mapping(record.mappings, mapping_key.capability, mapping_key.variable)
        core.remove_item(mapping.itemId)
    end
end

local output_params = { subscriptionRecord = new_record }
cloud_devices.store_subscription_record(output_params)

