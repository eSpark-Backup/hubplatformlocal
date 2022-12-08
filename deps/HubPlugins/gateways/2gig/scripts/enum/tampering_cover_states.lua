local enum = {
    ok = "no_tampering_cover",
    not_ok = "tampering_cover",
    unknown = "unknown"
}

function enum.from_bool(alarm)
    return alarm and enum.not_ok or enum.ok
end

return enum
