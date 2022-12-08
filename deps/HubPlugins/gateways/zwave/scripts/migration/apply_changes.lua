local node_id, storage_version, scripts_version = ...

local zwave = require("zwave")
local zwave_node = zwave.get_node(node_id)

local should_be_migrated = hubplugins.weak_loadfile("HUB:zwave/scripts/migration/should_be_migrated")
local process_node = hubplugins.weak_loadfile("HUB:zwave/scripts/migration/process_node")

local function apply_version_changes(version)
    local version_changes_file = hubplugins.weak_loadfile("HUB:zwave/scripts/migration/version_changes/" .. version)
    if version_changes_file then
        local changes = version_changes_file()

        assert(changes, "version changes " .. version .. " is nil")

        if should_be_migrated(zwave_node, changes) then
            print("[migration] migrating to version " .. version)
            process_node(
                zwave_node,
                changes.items_old_names,
                changes.storage_item_deleter,
                changes.storage_setting_deleter,
                changes.items_comparator,
                changes.items_to_change_value
            )
        end

        if changes.scene_modifications then
            local scenes = require "scenes"
            print("[migration] migrating scenes to version " .. version)

            for _, modification in ipairs(changes.scene_modifications) do
                if modification.commands and #modification.commands > 0 then
                    scenes.modify_scenes(modification.commands, modification.block_ids)
                else
                    print("[migration] invalid scene modification commands")
                end
            end
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
