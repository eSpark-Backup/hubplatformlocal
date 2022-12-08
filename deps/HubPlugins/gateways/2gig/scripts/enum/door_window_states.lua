local enum = {
    dw_is_closed = "dw_is_closed",
    dw_is_opened = "dw_is_opened",
    unknown = "unknown"
}

function enum.from_bool(is_closed)
    return is_closed and enum.dw_is_closed or enum.dw_is_opened
end

return enum
