require("HUB:zwave/scripts/helpers/scripts_preload")

local storage_helper = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

storage_helper:call("remove_value", "migration_table")
storage_helper:call("remove_value", "migration_table_index")

local version = hubplugins.weak_loadfile("HUB:zwave/scripts/resources/version")()
storage_helper:call("set_storage_version", version)

print("[migration] finalized migration. current version: " .. version)
