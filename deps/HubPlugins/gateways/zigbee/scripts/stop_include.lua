print("LUA: Try stop include...")

require ("HUB:zigbee/scripts/helpers/scripts_preload")

local zigbee = assert(require "zigbee", "Fail to load the zigbee functionality")

zigbee.stop_include()
