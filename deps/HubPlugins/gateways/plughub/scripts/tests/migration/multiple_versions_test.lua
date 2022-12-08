local tests = require("tests")
local storage = require("storage")

loadfile = tests.create_mock(function(path, ...)
    -- Note: assume the only time loadfile is called is to retrieve a migration script
    -- Todo: think of something if it's no longer true
    return {
        migrate_up = function() end,
        migrate_down = function() end
    }
end)

local real_require = require
local function set_target_version(version)
    require = function(path)
        if path == "HUB:plughub/scripts/resources/version" then
            return version
        else
            return real_require(path)
        end
    end
end

local function set_current_version(version)
    storage.set_number("plugin_version", version)
end

local function execute_migration(from, to)
    set_current_version(from)
    set_target_version(to)

    require("HUB:plughub/scripts/migration/execute_migration")
end

execute_migration(3, 6)
tests.assert_called(loadfile, "Check migrations run", {
    { "HUB:plughub/scripts/migration/versions/4" },
    { "HUB:plughub/scripts/migration/versions/5" },
    { "HUB:plughub/scripts/migration/versions/6" },
})

execute_migration(7, 2)
tests.assert_called(loadfile, "Check migrations run", {
    { "HUB:plughub/scripts/migration/versions/7" },
    { "HUB:plughub/scripts/migration/versions/6" },
    { "HUB:plughub/scripts/migration/versions/5" },
    { "HUB:plughub/scripts/migration/versions/4" },
    { "HUB:plughub/scripts/migration/versions/3" },
})
