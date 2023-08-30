local handler, setting_id, descriptor = ...

local core = assert(require "core", "Fail to load core functionality")
local zigbee = assert(require "zigbee", "Fail to load core functionality")

local setting = core.get_setting(setting_id)

if setting.value_default then
    core.set_setting_value(setting_id, setting.value_default, "synced")
    zigbee.request_attribute_value(
        descriptor.node_id, 
        descriptor.endpoint_id, 
        descriptor.cluster_name, 
        descriptor.attribute_name
    )
else
    print( "Unable to reset setting value, no default value specified" )
end
