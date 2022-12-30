local params = ...

require ("HUB:zigbee/scripts/helpers/scripts_preload")

if params and params.restrictedDeviceCategories then
    local restrictions_helper = hubplugins.object("include_restrictions_helper", "HUB:zigbee/scripts/helpers")
    restrictions_helper:call("restriction_added", params.restrictedDeviceCategories)
end

local zigbee = assert(require "zigbee", "Fail to load the zigbee functionality")
zigbee.start_include("HUB:zigbee/configs/interview_configurations.json")
