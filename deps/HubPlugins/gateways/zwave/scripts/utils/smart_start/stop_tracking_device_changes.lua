local self, device_id = ...

local core = assert( require( "core" ), "Fail to load the 'core' functionality" )

local core_subscriber_id = self:call( "get_subscriber_id" )

local device_rule = {}
device_rule.id = device_id
device_rule.event = ""

--ask core to stop notify us about events about changing this device's name and/or room
core.set_subscription_filters( core_subscriber_id, { device_rule } )
