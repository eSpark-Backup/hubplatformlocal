local device_info, tx_ids = ...

local core = require("core")
local storage_utils = require("HUB:2gig/scripts/helpers/storage_utils")

local gateway_id = core.get_gateway().id

local function process_device(device_info, tx_id)
    -- Since metatables are involved, create a new table with explicit indexing
    local core_device_id = core.add_device({
        gateway_id = gateway_id,
        parent_device_id = device_info.parent_device_id,
        name = device_info.name,
        category = device_info.category,
        subcategory = device_info.subcategory,
        type = device_info.type,
        device_type_id = device_info.device_type_id,
        info = device_info.info,
        battery_powered = device_info.battery_powered,
        reachable = device_info.reachable
    })

    storage_utils.register_device(tx_id, core_device_id)

    loadfile("HUB:2gig/scripts/helpers/add_items")(device_info, core_device_id)

    loadfile("HUB:2gig/scripts/helpers/add_settings")(device_info, core_device_id, tx_id)

    return core_device_id
end

local root_device_id = process_device(device_info, tx_ids[1])
device_info.id = root_device_id

-- Note: only 1 level of depth is supported
if device_info.child_devices ~= nil then
    for idx, device in ipairs(device_info.child_devices) do
        device.name = device_info.name .. " " .. idx
        device.parent_device_id = root_device_id

        device.id = process_device(device, tx_ids[idx + 1])
    end
end
