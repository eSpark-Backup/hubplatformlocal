local enum = {
    ok = "heat_ok",
    not_ok = "overheat_detected",
    unknown = "unknown"
}

function enum.from_bool(alarm)
    return alarm and enum.not_ok or enum.ok
end

return enum
