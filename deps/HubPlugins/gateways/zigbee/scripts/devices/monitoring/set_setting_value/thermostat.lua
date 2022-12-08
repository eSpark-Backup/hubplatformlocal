local handler, setting_id, descriptor, value = ...

local core = assert(require "core", "Fail to load core functionality")
local zigbee = assert(require "zigbee", "Fail to load core functionality")

local setting = core.get_setting(setting_id)

if setting.value_default then
    zigbee.set_attribute_value(
        descriptor.node_id, 
        descriptor.endpoint_id, 
        descriptor.cluster_name, 
        descriptor.attribute_name, 
        value * 10
    )
    
    zigbee.request_attribute_value(
        descriptor.node_id, 
        descriptor.endpoint_id, 
        descriptor.cluster_name, 
        descriptor.attribute_name
    )
else
    print( "Unable to reset setting value, no default value specified" )
end
