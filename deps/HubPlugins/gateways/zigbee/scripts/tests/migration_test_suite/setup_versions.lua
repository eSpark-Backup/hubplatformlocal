local storage, version_changes, old_version, new_version = ...

local original_loadfile = loadfile
local new_loadfile =  function (path)
    -- fake current version
    if path == "HUB:zigbee/scripts/resources/version" then
        return function() return new_version end
    -- fake current version changes
    elseif path == ("HUB:zigbee/scripts/migration/version_changes/" .. math.floor(new_version)) then
        return function() return version_changes end
    else
        return original_loadfile(path)
    end
end

storage.set_number("version", old_version)

return new_loadfile
