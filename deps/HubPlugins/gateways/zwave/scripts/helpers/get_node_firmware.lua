local zwave_node_info, version_cc, firmware_update_cc, region = ...

local firmware_versions = version_cc and version_cc.firmware_versions

if not firmware_versions then
    return nil
end

local result = {}

local firmware_ids = firmware_update_cc and firmware_update_cc.firmware_ids 
local use_firmware_ids = firmware_ids and #firmware_ids == #firmware_versions

region = region and region:lower() or "us"

for index, version in ipairs(firmware_versions) do
    local firmware = { version = version }

    if use_firmware_ids and firmware_ids[index] ~= 0 then
        firmware.id = ("%s.%g.%g.%g"):format(region, zwave_node_info.manufacturer_id, firmware_ids[index], index - 1)
    else
        firmware.id = ("%s.%g.%g_%g.%g"):format(region, zwave_node_info.manufacturer_id, zwave_node_info.product_type, zwave_node_info.product_id, index - 1)
    end

    table.insert( result, firmware )
end

return result