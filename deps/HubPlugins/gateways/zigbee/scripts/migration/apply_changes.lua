local node_id, storage_version, scripts_version = ...

local zigbee = require("zigbee")
local zigbee_node = zigbee.get_node(node_id)

local should_be_migrated = hubplugins.weak_loadfile("HUB:zigbee/scripts/migration/should_be_migrated")

local function apply_version_changes(version)
    local version_changes_file = hubplugins.weak_loadfile("HUB:zigbee/scripts/migration/version_changes/" .. version)
    if version_changes_file then
        local changes = version_changes_file()

        if should_be_migrated(zigbee_node, changes) then
            hubplugins.weak_loadfile("HUB:zigbee/scripts/migration/process_node")(
                zigbee_node,
                changes.items_old_names,
                changes.storage_item_deleter,
                changes.should_use_old_item_value
            )
        end
    else
        print("[migration] no version: " .. version)
    end
end


-- main


if storage_version == scripts_version then
    -- database is corrupted, check all
    apply_version_changes("check_all")
else
    for i = storage_version + 1, scripts_version, 1 do
        apply_version_changes(math.floor(i))
    end
end
