local core = assert(require("core"), "Failed to load core functionality")

local item_infos = {
    electric_meter_amper = { value_type = "electric_current",        scale = "ampere" },
    electric_meter_kwh =   { value_type = "amount_of_useful_energy", scale = "kilo_watt_hour" },
    electric_meter_volt =  { value_type = "electric_potential",      scale = "volt" },
    electric_meter_watt =  { value_type = "power",                   scale = "watt" },
}

local function for_each_affected_item(callback)
    local items = assert(core.get_items(), "Error loading items")

    for _, item in ipairs(items) do
        local item_info = item_infos[item.name]
        if item_info then
            callback(item, item_info)
        end
    end
end

local routine = {}

routine.migrate_up = function()
    for_each_affected_item(function(item, item_info)
        item.value_type = item_info.value_type
        item.value = {
            value = item.value,
            scale = item_info.scale
        }

        core.modify_item(item.id, item)
    end)
end

routine.migrate_down = function()
    for_each_affected_item(function(item, item_info)
        item.value_type = "float"
        item.value = item.value.value

        core.modify_item(item.id, item)
    end)
end

return routine
