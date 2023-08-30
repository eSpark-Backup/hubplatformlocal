local tests = require("tests")
local fixture = require("HUB:2gig/scripts/tests/migration/migration_fixture")
local item_names = require("HUB:2gig/scripts/enum/item_names")

local function create_device()
    local device = {
        name = "Door & Window Sensor",
        device_type = "sensor",
        device_type_id = "2GIG-DW10-345",
        type = "sensor",
        category = "security_sensor",
        subcategory = "door",
        battery_powered = true,
        reachable = true,
        info = {manufacturer = '2GIG', model = 'DW10-345'}
    }

    function device.create_items()
        local item_factory = require("HUB:2gig/scripts/helpers/item_factory")

    return {
        item_factory.create_door_window_state_item(),
        item_factory.create_security_threat_item(),
        item_factory.create_tampering_cover_alarm_item(),
        item_factory.create_battery_item(),
    }
    end

    return fixture.add_device(device, { 123 })
end

local device = create_device()

do
    tests.assert_eq(fixture.get_settings_count(device.id), 0)
end

fixture.migrate_to(10)

do
    tests.assert_eq(fixture.get_settings_count(device.id), 1)
    fixture.check_setting_exists(device.id, "dw_detection_source")
end
