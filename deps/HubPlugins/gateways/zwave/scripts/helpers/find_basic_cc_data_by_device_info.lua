local zwave_node = ...

local basic_cc_data_by_device_info = hubplugins.weak_loadfile("HUB:zwave/scripts/resources/basic_cc_data_by_device_info")()
local generic_types_to_cc_data = basic_cc_data_by_device_info.generic_types_to_cc_data
local icon_types_to_cc_data = basic_cc_data_by_device_info.icon_types_to_cc_data

local dev_handler_id = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/create_device_type_id")(zwave_node.manufacturer_id, zwave_node.product_type, zwave_node.product_id)
local device_info = hubplugins.weak_loadfile("HUB:zwave/scripts/model/devices/devices_info/generated/" .. dev_handler_id)
if device_info then device_info = device_info() end

local generic_type = ( device_info and device_info.generic_type ) or zwave_node.generic_type
local cc_data = generic_type and generic_types_to_cc_data[ generic_type ]

if not cc_data and zwave_node.icon_type then
    cc_data = icon_types_to_cc_data[zwave_node.icon_type]
end

if not cc_data then
    return nil
end

local result = { id = cc_data.class_id }

local is_manufacturer_undefined = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/is_manufacturer_undefined")(zwave_node.manufacturer_id)

--[[    addon for previous  certification
if is_manufacturer_undefined then
    local zwave = require "zwave"
    local class_ids = require("HUB:zwave/scripts/resources/class_ids")

    if result.id == class_ids.switch_binary and zwave.get_command_class(zwave_node.id, 0, class_ids.switch_binary) == nil then
        result.id = class_ids.dimmer
    end
end

--]]

if cc_data.subcategory_to_sensor_type and device_info then
    result.sensor_type = device_info.main_sensor_type or cc_data.subcategory_to_sensor_type[ device_info.subcategory ]
end

return result
