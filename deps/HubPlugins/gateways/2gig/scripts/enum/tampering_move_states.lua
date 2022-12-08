local enum = {
    ok = "no_moving",
    not_ok = "product_moved",
    unknown = "unknown"
}

function enum.from_bool(alarm)
    return alarm and enum.not_ok or enum.ok
end

return enum
