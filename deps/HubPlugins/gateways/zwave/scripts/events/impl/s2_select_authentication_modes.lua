local params = ...

print("Supported authentication modes:")
for _, mode in ipairs(params.modes) do
    print("- " .. mode)
end

local broadcasts_repeater = hubplugins.object("broadcasts_repeater", "HUB:zwave/scripts/utils")
broadcasts_repeater:call("start", { event = params.event, modes = params.modes })
