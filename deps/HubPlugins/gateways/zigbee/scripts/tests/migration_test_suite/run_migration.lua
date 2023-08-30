loadfile("HUB:zigbee/scripts/migration/initialize_migration")()

local migrate_device = loadfile("HUB:zigbee/scripts/migration/migrate_device")

while( migrate_device() ) do end

loadfile("HUB:zigbee/scripts/migration/finalize_migration")()
