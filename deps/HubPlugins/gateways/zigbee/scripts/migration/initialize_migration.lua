require ("HUB:zigbee/scripts/helpers/scripts_preload")

local storage_helper = hubplugins.object("storage_helper", "HUB:zigbee/scripts/utils")

local migration_table = storage_helper:call("get_table", "migration_table")

if not migration_table then
    migration_table = {}
    local zigbee = require("zigbee")
    local nodes = zigbee.get_all_nodes_ids()
    for _, node_id in ipairs(nodes) do
        table.insert(
            migration_table,
            {
                node_id = node_id,
                attempt_to_migrate = 0
            }
        )
    end

    storage_helper:call("set_table", "migration_table", migration_table)
    storage_helper:call("set_number", "migration_table_index", 1)

    print( storage_helper:call("get_number", "migration_table_index") )
end

print("[migration] initialized migration")
