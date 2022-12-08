local enum = {
    ok = "no_glass_breakage",
    not_ok = "glass_breakage",
    unknown = "unknown"
}

function enum.from_bool(alarm)
    return alarm and enum.not_ok or enum.ok
end

return enum
