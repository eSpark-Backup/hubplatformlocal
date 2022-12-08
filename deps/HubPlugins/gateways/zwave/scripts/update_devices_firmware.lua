local params = ...

require ("HUB:zwave/scripts/helpers/scripts_preload")

local zwave = require "zwave"

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

local node_ids = {}

local fw_ids_info = {}
for _, target_firmware in ipairs(params.firmware) do
    fw_ids_info[target_firmware.id] = {}
end

local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )

local region = zwave.get_firmware_region()

for _, device_id in ipairs(params.devices) do
    local node_id = storage:call("find_node_id", device_id)
    assert(node_id, "cannot find node")

    local node_firmware_list = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_node_firmware")(
            zwave.get_node_info(node_id), 
            zwave.get_command_class(node_id, 0, class_ids.version),
            zwave.get_command_class(node_id, 0, class_ids.firmware_update),
            region
        )

    assert(node_firmware_list, "cannot find firmware list for node " .. node_id)

    local fw_id_to_target = {}
    for index, firmware in ipairs(node_firmware_list) do
        fw_id_to_target[firmware.id] = index - 1
    end

    for id, info in pairs(fw_ids_info) do
        local target = fw_id_to_target[id]
        if not target then
            error("Cannot find firmware " .. id .. " in device " .. device_id)
        elseif info.target and info.target ~= target then
            error("Firmware " .. id .. " has wrong target for device " .. device_id)
        else
            info.target = target
        end
    end

    table.insert(node_ids, node_id)
end


for _, target_firmware in ipairs(params.firmware) do
    target_firmware.target = fw_ids_info[target_firmware.id].target

    if not target_firmware.target then
        error("Cannot find target for firmware " .. target_firmware.id)
    end

    target_firmware.id = nil
end

zwave.update_nodes_firmware(node_ids, params.firmware)
