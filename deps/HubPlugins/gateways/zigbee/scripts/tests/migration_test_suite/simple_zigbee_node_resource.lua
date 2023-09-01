local cluster_on_off = { 
    name = "on_off",
    attributes = {
        {
            name = "OnOff",
            formatted_value = "false"
        }
    }
}

local endpoint_1 = { 
    id = 1,
    clusters = { cluster_on_off }
}

return {
    id = 42,
    manufacturer_id = "Ezlo",
    model_id = "Plug",
    security = 0,
    is_reachable = 1,
    is_battery_powered = false,

    endpoints = { endpoint_1 }
}
