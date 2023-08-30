local core = require("core")
local tests = require("tests")
local fixture = require("HUB:2gig/scripts/tests/migration/migration_fixture")
local storage_utils = require("HUB:2gig/scripts/helpers/storage_utils")
local migration_utils = require("HUB:2gig/scripts/migration/migration_utils")
local item_names = require("HUB:2gig/scripts/enum/item_names")

local function create_device(device)
    function device.create_items()
        local item_factory = require("HUB:2gig/scripts/helpers/item_factory")

        return {
            item_factory.create_co_alarm_item(),
            item_factory.create_tampering_cover_alarm_item(),
            item_factory.create_battery_item(),
        }
    end

    device.gateway_id = core.get_gateway().id
    device.id = core.add_device(device)

    storage_utils.register_device(123, device.id)

    loadfile("HUB:2gig/scripts/helpers/add_items")(device, device.id)

    return device
end

local function create_co_detector(use_model)
    local device = {
        name = "CO Detector",
        device_type = "co_detector",
        device_type_id = "co_detector",
        type = "sensor",
        category = "security_sensor",
        subcategory = "co",
        battery_powered = true,
        reachable = true
    }

    if use_model then
        device.info = { manufacturer = '2GIG', model = 'CO3-345' }
        device.device_type_id = "2GIG-CO3-345"
    end

    return create_device(device)
end

local function create_smoke_detector(use_model)
    local device = {
        name = "Smoke Detector",
        device_type = "smoke_detector",
        device_type_id = "smoke_detector",
        type = "sensor.multisensor",
        category = "security_sensor",
        subcategory = "smoke",
        battery_powered = true,
        reachable = true
    }

    if use_model then
        device.info = { manufacturer = '2GIG', model = 'SMKT3-345' }
        device.device_type_id = '2GIG-SMKT3-345'
    end

    return create_device(device)
end

local smoke_device1 = create_smoke_detector(true)
local smoke_device2 = create_smoke_detector(false)
local co_device1 = create_co_detector(true)
local co_device2 = create_co_detector(false)

local device_filter = {
    co_detector = true
}

local expected_matches = {
    [co_device1.id] = true,
    [co_device2.id] = true
}

local unexpected_matches = {}

migration_utils.for_each_device(device_filter, function(device)
    if expected_matches[device.id] ~= nil then
        expected_matches[device.id] = nil
    else
        table.insert(unexpected_matches, device.id)
    end
end)

tests.assert_eq(next(expected_matches), nil, "All expected devices must be matched")
tests.assert_eq(next(unexpected_matches), nil, "No unexpected devices must be matched")
