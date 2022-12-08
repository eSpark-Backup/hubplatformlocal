local enum = {
    ok = "no_freeze",
    not_ok = "freeze_detected",
    unknown = "unknown"
}

function enum.from_bool(alarm)
    return alarm and enum.not_ok or enum.ok
end

return enum
