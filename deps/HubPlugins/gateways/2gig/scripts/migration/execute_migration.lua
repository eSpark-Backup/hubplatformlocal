local storage = assert(require("storage"), "Failed to load storage functionality")

local function migrate_to(version)
    local migration_script = loadfile("HUB:2gig/scripts/migration/versions/" .. math.floor(version))
    assert(migration_script, ("Migration to version %d is not implemented"):format(version))

    migration_script()
end

local current_version = storage.get_number("plugin_version") or 1
local target_version = require("HUB:2gig/scripts/resources/version")

if current_version ~= target_version then
    assert(current_version < target_version, "[Migration] Migration from a newer version is not supported")

    local message = "[Migration] Migration from version %d up to version %d is required"
    print(message:format(current_version, target_version))

    for version = current_version + 1, target_version do
        print(("[Migration] Migrating plugin up to version %d"):format(version))
        migrate_to(version)
    end

    storage.set_number("plugin_version", target_version)
end

print("[Migration] Plugin is up-to-date")

return false
