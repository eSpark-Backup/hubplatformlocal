local fixture = {}

fixture.get_migration_routine = function(version)
    return require("HUB:plughub/scripts/migration/versions/" .. version)
end

return fixture
