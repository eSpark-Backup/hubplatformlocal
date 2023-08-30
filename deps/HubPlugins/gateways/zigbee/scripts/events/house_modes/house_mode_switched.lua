local params = ...

print(
    string.format(
        "House mode changed, from: %s, to: %s, status: %s, switch_delay: %d",
        params.from,
        params.to,
        params.status,
        params.switch_delay
    )
)

local core = assert( require "core", "Fail to load the zigbee functionality" )
local zigbee = assert( require "zigbee", "Fail to load the zigbee functionality" )
local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")
local storage = hubplugins.object("storage_helper", "HUB:zigbee/scripts/utils")

local items = core.get_items()
for _, item in ipairs(items) do
    if item.name == item_names.zones_status then
        local descriptor = storage:call("find_item_descriptor", item.id)
        if not descriptor then
            print( "Item descriptor not found with id: " .. item.id)
            return
        end

        local node = zigbee.get_node(descriptor.node_id)

        local dev_handler = hubplugins.weak_loadfile("HUB:zigbee/scripts/helpers/get_device_handler_monitoring")(node)
        dev_handler:house_mode_switched(
            item.id,
            descriptor,
            params.from,
            params.to,
            params.status,
            params.switch_delay
        )
    end
end
