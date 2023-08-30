local core = require("core")
local tests = require("tests")
local storage_utils = require("HUB:2gig/scripts/helpers/storage_utils")

local fixture = {}

fixture.add_device = function(device, tx_ids)
    loadfile("HUB:2gig/scripts/helpers/add_device")(device, tx_ids)

    return device, table.unpack(device.child_devices or {})
end

fixture.migrate_to = function(version)
    print("Migrating to version " .. version)
    require("HUB:2gig/scripts/migration/versions/" .. version)
end

fixture.find_item_in_core = function(device_id, item_name)
    local items = core.get_items_by_device_id(device_id) or {}

    for _, item in pairs(items) do
        if item.name == item_name then
            return item
        end
    end
end

fixture.find_item_in_storage = function(device_id, item_name, tag)
    return storage_utils.find_item_id(device_id, item_name, tag)
end

fixture.find_setting_in_core = function(device_id, setting_tag)
    local setting_ids = core.get_setting_ids_by_device_id(device_id) or {}

    for _, setting_id in pairs(setting_ids) do
        local setting = core.get_setting(setting_id)

        if setting.info.tag == setting_tag then
            return setting
        end
    end
end

fixture.find_setting_in_storage = function(device_id, setting_tag)
    return storage_utils.find_setting_id(device_id, setting_tag)
end

fixture.get_items_count = function(device_id)
    local items = core.get_items_by_device_id(device_id)

    return #items
end

fixture.get_settings_count = function(device_id)
    local setting_ids = core.get_setting_ids_by_device_id(device_id) or {}

    return #setting_ids
end

fixture.check_item_exists = function(device_id, item_name)
    local item = fixture.find_item_in_core(device_id, item_name)
    tests.assert_ne(item, nil)

    local item_id = fixture.find_item_in_storage(device_id, item_name)
    tests.assert_ne(item_id, nil)

    tests.assert_eq(item.id, item_id)
end

fixture.check_item_doesnt_exist = function(device_id, item_name)
    tests.assert_eq(fixture.find_item_in_core(device_id, item_name), nil)
    tests.assert_eq(fixture.find_item_in_storage(device_id, item_name), nil)
end

fixture.check_setting_exists = function(device_id, setting_tag)
    local setting = fixture.find_setting_in_core(device_id, setting_tag)
    tests.assert_ne(setting, nil)

    local setting_id = fixture.find_setting_in_storage(device_id, setting_tag)
    tests.assert_ne(setting_id, nil)

    tests.assert_eq(setting.id, setting_id)
end

fixture.check_setting_doesnt_exist = function(device_id, setting_tag)
    tests.assert_eq(fixture.find_setting_in_core(device_id, setting_tag), nil)
    tests.assert_eq(fixture.find_setting_in_storage(device_id, setting_tag), nil)
end

return fixture
