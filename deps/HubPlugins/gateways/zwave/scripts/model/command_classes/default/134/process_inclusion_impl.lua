local self, inclusion_context, build_tree_context, device_node = ...


local zwave = require "zwave"
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )

local cc_version = build_tree_context.command_class

local cc_firmware_update
for _, channel in ipairs(inclusion_context.zwave_node.channels) do
    if channel.id == 0 then
        for _, cc in ipairs(channel.classes) do
            if cc.id == class_ids.firmware_update then
                cc_firmware_update = cc
                break
            end
        end
        break
    end
end

local firmware_list = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_node_firmware")(
                                                inclusion_context.zwave_node, 
                                                cc_version,
                                                cc_firmware_update,
                                                zwave.get_firmware_region and zwave.get_firmware_region())

if cc_firmware_update and firmware_list then
    print("Firmware is updatable, add firmware(" .. #firmware_list .. ") list to device info")
end

local root = inclusion_context.device_tree_model:call("get_root_node")
local info = inclusion_context.device_tree_model:call("ensure_device_info_unique_fields", root, "info")

info.info.hardware = cc_version.hardware_version and tostring(math.floor(cc_version.hardware_version))
info.info["firmware.stack"] = cc_version.zwave_protocol_version
info.firmware = firmware_list


