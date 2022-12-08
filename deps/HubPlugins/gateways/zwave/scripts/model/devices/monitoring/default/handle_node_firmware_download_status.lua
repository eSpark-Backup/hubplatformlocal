local self, image, checksum, status, progress, error_desription = ...

local core = require "core"

core.send_device_firmware_download_broadcast({
    status = status,
    progress = progress,
    image = image,
    checksum = checksum,
    error = error_desription
})
