local self, device_data, zwave_node = ...

local zwave = require("zwave")
local core = require("core")
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

do
    local setting = {
        label = {
            text = "Rediscover device"
        },
        description = {
            text = "Rediscover (repeat interview) device"
        },
        value_type = "action",
        value = {
            text = "Rediscover",
            lang_tag = "rediscover_tag"
        },
        descriptor = {
            node_id = zwave_node.id,
            class_id = nil,
            tag = "rediscover"
        }
    }

    table.insert(device_data.settings, setting)
end

do
    self:call("process_associations/create_associations_settings", device_data, zwave_node)
end
