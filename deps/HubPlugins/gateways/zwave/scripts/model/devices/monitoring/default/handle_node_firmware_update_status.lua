local self, node_id, image, target, status, progress, error_desription = ...

local core = require "core"
local zwave = require "zwave"

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

local device_id = storage:call("find_device_id", node_id)

assert(device_id, "node device not found for node " .. node_id)

if target then
    local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )

    local firmware_list = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_node_firmware")(
            zwave.get_node_info(node_id), 
            zwave.get_command_class(node_id, 0, class_ids.version),
            zwave.get_command_class(node_id, 0, class_ids.firmware_update),
            zwave.get_firmware_region()
        )

    assert(firmware_list, "Firmware not found for node " .. node_id)

    local firmware = firmware_list[ target + 1 ]
    local firmware_id = firmware and firmware.id

    if status == "started" then
        core.update_device_status(device_id, "updating")
    end

    core.send_device_firmware_update_broadcast({
        status = status,
        progress = progress,
        device_id = device_id,
        firmware_id = firmware_id,
        image = image,
        error = error_desription
    })
else
    core.update_device_status(device_id, 
        status == "waiting_for_node_restart" and "waiting_for_restart" or
        status == "finished"                 and "rediscovering" or
        status == "failed"                   and "idle" or 
                                                 "updating" )

    if status == "finished" then
        print("request rediscover starting")
        
        zwave.start_rediscover(node_id, "HUB:zwave/configs/device_setting.json")
    elseif status == "waiting_for_node_restart" then
        print("Update status is waiting_for_restart! How to handle it?")
    end
end
