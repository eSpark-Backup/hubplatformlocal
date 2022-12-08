loadfile("HUB:zwave/scripts/migration/initialize_migration")()

local migrate_device = loadfile("HUB:zwave/scripts/migration/migrate_device")

while( migrate_device() ) do end

loadfile("HUB:zwave/scripts/migration/finalize_migration")()
