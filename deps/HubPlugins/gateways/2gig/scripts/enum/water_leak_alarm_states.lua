local enum = {
    ok = "no_water_leak",
    not_ok = "water_leak_detected",
    unknown = "unknown"
}

function enum.from_bool(alarm)
    return alarm and enum.not_ok or enum.ok
end

return enum
