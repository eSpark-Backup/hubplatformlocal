local check_context = ...

local tests = require("tests")

local core = require("core")
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

tests.assert_eq(next(core.items), nil, "all items removed from core")
tests.assert_eq(next(core.devices), nil, "all devices removed from core")
tests.assert_eq(next(core.settings), nil, "all settings removed from core")

if check_context.item_id then
    for _, item_id in ipairs(check_context.item_ids) do
        tests.assert_eq(storage:call("find_item_descriptors", item_id), nil, "descriptors->ids must be removed")
        tests.assert_eq(core.get_item(item_id), nil, "item must be removed from core" )
    end
end

if check_context.setting_ids then
    for _, setting_id in ipairs(check_context.setting_ids) do
        tests.assert_eq(storage:call("find_setting_descriptor", setting_id), nil, "setting id must be removed")
        tests.assert_eq(core.get_setting(setting_id), nil, "setting must be removed from core" )
    end
end

for _, descriptor in ipairs(check_context.descriptors) do
    local id =
        storage:call("find_item_id",
            descriptor.item_name,
            descriptor.node_id,
            descriptor.class_id,
            descriptor.channel_id,
            descriptor.tag
        )

    tests.assert_eq(id , nil, "ids to descriptors must be removed" )

    if not check_context.skip_node_id_to_device_check then
        local device_id = storage:call("find_node_id", descriptor.node_id)
        tests.assert_eq(device_id , nil, "node_id to device_id must be removed")
    end
end

for _, device_id in ipairs(check_context.device_ids) do
    local node_id = storage:call("find_node_id", device_id)
    tests.assert_eq(node_id , nil, "device_id to node_id must be removed")
    tests.assert_eq(core.get_device( device_id ), nil, "device must be removed from core")
end
