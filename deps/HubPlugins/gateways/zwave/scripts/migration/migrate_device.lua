require("HUB:zwave/scripts/helpers/scripts_preload")

local core = require("core")
local storage_helper = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

print( "[migration] checking if migration is required..." );

local current_storage_version =  storage_helper:call("get_storage_version") or 1
local target_storage_version = hubplugins.weak_loadfile("HUB:zwave/scripts/resources/version")()

local migration_table_index = storage_helper:call("get_number", "migration_table_index")
local migration_table = storage_helper:call("get_table", "migration_table")
local node_info = migration_table[migration_table_index]

if not node_info then
    print("[migration] all nodes migrated")
    return false
end

if target_storage_version == current_storage_version then
    -- Migrate device if no device found by node id
    local device_id = storage_helper:call("find_device_id", node_info.node_id)
    if device_id and core.get_device(device_id) then
        print( "[migration] data format is up-to-date, nothing to do" );
        return false
    end
end

node_info.attempt_to_migrate = node_info.attempt_to_migrate + 1
storage_helper:call("set_table", "migration_table", migration_table)

print(
    ("[migration] start migration process for %d version to %d, node id: %d, attempt to migrate: %d"):format(
        current_storage_version,
        target_storage_version,
        node_info.node_id,
        node_info.attempt_to_migrate)
)

local attempt_to_migrate_limit = 3

local status, retval

if node_info.attempt_to_migrate < attempt_to_migrate_limit then
    status, retval = xpcall(hubplugins.weak_loadfile("HUB:zwave/scripts/migration/apply_changes"), debug.traceback, node_info.node_id, current_storage_version, target_storage_version)
    if not status then
        print("[migration] fail to apply changes: \n" .. retval)
        return true -- try migration again
    end
end

if node_info.attempt_to_migrate == attempt_to_migrate_limit then
    print("[migration] make devices broken by node id: " .. tostring(node_info.node_id))

    status, retval = xpcall(hubplugins.weak_loadfile("HUB:zwave/scripts/migration/make_devices_broken"), debug.traceback, node_info.node_id)
    if not status then
        print("[migration] fail to make devices broken: \n" .. retval)
    end
end

migration_table_index = migration_table_index + 1
storage_helper:call("set_number", "migration_table_index", migration_table_index)

return true
