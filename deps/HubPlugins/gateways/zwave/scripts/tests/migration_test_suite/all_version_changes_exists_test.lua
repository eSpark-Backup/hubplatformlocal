local current_version = loadfile("HUB:zwave/scripts/resources/version")()

for i = 2, current_version, 1 do
    local version_changes_file = loadfile("HUB:zwave/scripts/migration/version_changes/" .. i)
    assert(version_changes_file, "File with version changes should be exist for version: " .. i)
    local changes = version_changes_file()
    if not changes.check_all_nodes and next(changes.cc_to_check) == nil and next(changes.devices_to_check) == nil then
        error("No changes exist for " .. 
            i .. 
            " version. At least 'check_all_nodes' or 'cc_to_check' should be specified."
        )
    end
end
