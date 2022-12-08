local self, device_id = ...

local core = assert( require( "core" ), "Fail to load the 'core' functionality" )

local core_subscriber_id = self:call( "get_subscriber_id" )

local device_rule = {}
device_rule.id = device_id
device_rule.event = "device_updated"
device_rule.accepts = { { _id = device_id, name = {} }, { _id = device_id, roomId = {} } }

--ask core to notify us about events about changing device's name and/or room
core.set_subscription_filters( core_subscriber_id, { device_rule } )
