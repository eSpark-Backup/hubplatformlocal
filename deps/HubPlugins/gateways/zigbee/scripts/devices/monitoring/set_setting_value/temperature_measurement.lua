local handler, setting_id, descriptor, value = ...

local core = assert(require "core", "Fail to load core functionality")
local zigbee = assert(require "zigbee", "Fail to load core functionality")

local setting = core.get_setting(setting_id)

local value_scalable = setting.value
value_scalable.value = value

core.set_setting_value(setting_id, value_scalable, "synced")

zigbee.request_attribute_value(
    descriptor.node_id, 
    descriptor.endpoint_id, 
    descriptor.cluster_name, 
    descriptor.attribute_name
)
