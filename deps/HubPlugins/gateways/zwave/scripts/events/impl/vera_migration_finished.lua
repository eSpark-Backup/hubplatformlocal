local params = ...

if not params.vera_device_model then
    print("No Vera devices model received")
    return
end

local vera_migration_helper = hubplugins.object("vera_migration_helper", "HUB:zwave/scripts/helpers")
vera_migration_helper:call("generate_device_mappings", params.vera_device_model)
