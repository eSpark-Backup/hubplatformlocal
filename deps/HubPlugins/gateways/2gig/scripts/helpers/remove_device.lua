local tx_ids = ...

local core = require("core")
local storage_utils = require("HUB:2gig/scripts/helpers/storage_utils")

for idx, txid in ipairs(tx_ids) do
    local core_device_id = storage_utils.find_device_id(txid)

    if idx == 1 then
        -- Removing the root device removes the subtree as well
        local root_device_id = core.get_root_device_id(core_device_id)
        core.remove_device(root_device_id)
    end

    storage_utils.unregister_device(txid, core_device_id)
    storage_utils.unregister_items(core_device_id)
    storage_utils.unregister_settings(core_device_id)
end

loadfile("HUB:2gig/scripts/helpers/pin_codes_restrictions/process_restrictions")()
