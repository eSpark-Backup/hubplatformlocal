local check_context = ...

local tests = require("tests")

local core = require("core")
local storage_helper = hubplugins.object("storage_helper", "HUB:zigbee/scripts/utils")

tests.assert_eq(next(core.items), nil, "all items removed from core")
tests.assert_eq(next(core.devices), nil, "all devices removed from core")

for _, item_id in ipairs(check_context.item_ids) do
    tests.assert_eq(storage_helper:call("find_item_descriptors", item_id), nil, "descriptors->ids must be removed")
    tests.assert_eq(storage_helper:call("get_table", storage_helper:call("get_usernames_tablename", item_id)), nil, "user names table should be removed")
    tests.assert_eq(core.get_item(item_id), nil, "item must be removed from core" )
end

if check_context.setting_ids then
    for _, setting_id in ipairs(check_context.setting_ids) do
        tests.assert_eq(storage_helper:call("find_setting_descriptor", setting_id), nil, "setting id must be removed")
        tests.assert_eq(core.get_setting(setting_id), nil, "setting must be removed from core" )
    end
end

for _, descriptor in ipairs(check_context.descriptors) do
    local id =
        storage_helper:call(
            "find_item_id",
            descriptor
        )

    tests.assert_eq(id , nil, "ids to descriptors must be removed" )

    local device_id = storage_helper:call("find_node_id", descriptor.node_id)
    tests.assert_eq(device_id , nil, "node_id to device_id must be removed")
end

for _, device_id in ipairs(check_context.device_ids) do
    local node_id = storage_helper:call("find_node_id", device_id)
    tests.assert_eq(node_id , nil, "device_id to node_id must be removed")
    tests.assert_eq(core.get_device( device_id ), nil, "device must be removed from core")
end
