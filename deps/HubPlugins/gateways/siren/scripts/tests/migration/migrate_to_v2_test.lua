local core = require("core")
local storage = require("storage")
local tests = require("tests")
local storage_keys_getter = loadfile("HUB:siren/scripts/utils/storage_keys_getter")()
local item_names = assert(loadfile("HUB:siren/scripts/items/names")(), "Failed to load item_names")
local logger = require("HUB:siren/scripts/utils/log")

local fixture = {}

fixture.migrate_to = function(version)
    logger.info("Migrating to version " .. version)
    loadfile("HUB:siren/scripts/migration/versions/v" .. tostring(math.floor(version)))()
end


local function make_device()
    local device = {
        name = "Controller Siren",
        type = "siren",
        category = "siren",
        subcategory = "",
        security = "high",
        persistent = true,
        battery_powered = false,
        device_type_id = "e550_siren"
    }
    device.gateway_id = core.get_gateway().id
    return device
end

local function create_device(device)
    device.id = core.add_device(device)

    local main_item_name = item_names.switch
    local device_storage_key = storage_keys_getter.get_device_key(main_item_name)
    storage.set_string(device_storage_key, device.id)

    return device
end


local function test_empty_house_modes_device()
    local device = create_device(make_device())

    tests.assert_eq(device.house_modes_options, nil)

    storage.delete('plugin_version')

    fixture.migrate_to(2)

    local stored_plugin_version = storage.get_number('plugin_version') or 1

    tests.assert_eq(stored_plugin_version, 2, "Should have stored plugin_version=2")

    local main_item_name = item_names.switch
    local device_storage_key = storage_keys_getter.get_device_key(main_item_name)

    tests.assert_eq( storage.get_string(device_storage_key), device.id )

    local new_device = core.get_device( device.id)
    tests.assert_ne(new_device.house_modes_options, nil, "Should have house_modes_options key")

    local options = new_device.house_modes_options

    tests.assert_ne( options.alarms_off, nil, "Should have house_modes_options.alarms_off array" )
    tests.assert_eq( #options.alarms_off, 4 )

    core.remove_device( device.id )
    storage.delete('plugin_version')
end

test_empty_house_modes_device()
