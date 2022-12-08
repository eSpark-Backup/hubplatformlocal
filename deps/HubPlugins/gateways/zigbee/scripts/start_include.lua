print("LUA: Try start include...")

require ("HUB:zigbee/scripts/helpers/scripts_preload")

local zigbee = assert(require "zigbee", "Fail to load the zigbee functionality")

zigbee.start_include("HUB:zigbee/configs/interview_configurations.json")
