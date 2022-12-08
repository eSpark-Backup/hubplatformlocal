local params = ...

require ("HUB:zigbee/scripts/helpers/scripts_preload")

local zigbee = assert(require("zigbee"), "Fail to load the 'zigbee' functionality")

if params.attribute_name ~= "" then
    zigbee.request_attribute_value(
        params.node_id,
        params.endpoint_id,
        params.cluster_name,
        params.attribute_name
    )
end
