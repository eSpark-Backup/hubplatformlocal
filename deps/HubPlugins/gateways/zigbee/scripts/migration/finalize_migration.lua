require ("HUB:zigbee/scripts/helpers/scripts_preload")

local storage_helper = hubplugins.object("storage_helper", "HUB:zigbee/scripts/utils")

storage_helper:call("remove_value", "migration_table")
storage_helper:call("remove_value", "migration_table_index")

local version = hubplugins.weak_loadfile("HUB:zigbee/scripts/resources/version")()
storage_helper:call("set_storage_version", version)

print("[migration] finalized migration")
