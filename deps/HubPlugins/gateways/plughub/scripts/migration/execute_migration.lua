local storage = assert(require("storage"), "Failed to load storage functionality")

local function get_migration_routine(version)
    local migration_script = loadfile("HUB:plughub/scripts/migration/versions/" .. math.floor(version))
    assert(migration_script, ("Migration to version %d is not implemented"):format(version))

    return migration_script()
end

local current_version = storage.get_number("plugin_version") or 1
local target_version = require("HUB:plughub/scripts/resources/version")

if current_version ~= target_version then
    if current_version < target_version then
        print(("[Migration] Migration from version %d up to version %d is required"):format(current_version, target_version))

        for version = current_version + 1, target_version do
            print(("[Migration] Migrating plugin up to version %d"):format(version))
            get_migration_routine(version).migrate_up()
        end
    else
        print(("[Migration] Migration from version %d down to version %d is required"):format(current_version, target_version))

        for version = current_version, target_version + 1, -1 do
            print(("[Migration] Migrating plugin down to version %d"):format(version))
            get_migration_routine(version).migrate_down()
        end
    end

    storage.set_number("plugin_version", target_version)
end

print("[Migration] Plugin is up-to-date")

return false
