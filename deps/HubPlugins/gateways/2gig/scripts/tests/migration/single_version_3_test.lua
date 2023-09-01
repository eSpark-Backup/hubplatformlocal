local tests = require("tests")
local fixture = require("HUB:2gig/scripts/tests/migration/migration_fixture")
local item_names = require("HUB:2gig/scripts/enum/item_names")

local function create_device()
    local device = {
        name = "Tilt Sensor",
        device_type = "tilt_detector",
        device_type_id = "tilt_detector",
        type = "sensor",
        category = "generic_sensor",
        subcategory = "",
        battery_powered = true,
        reachable = true
    }

    function device.create_items()
        local item_factory = require("HUB:2gig/scripts/helpers/item_factory")

        return {
            item_factory.create_tampering_move_alarm_item(),
            item_factory.create_security_threat_item(),
            item_factory.create_tampering_cover_alarm_item(),
            item_factory.create_battery_item(),
        }
    end

    return fixture.add_device(device, { 123 })
end

local device = create_device()

do
    tests.assert_eq(fixture.get_items_count(device.id), 4)

    fixture.check_item_exists(device.id, item_names.tampering_move_alarm)
    fixture.check_item_doesnt_exist(device.id, item_names.door_window_state)
end

fixture.migrate_to(3)

do
    tests.assert_eq(fixture.get_items_count(device.id), 4)

    fixture.check_item_exists(device.id, item_names.door_window_state)
    fixture.check_item_doesnt_exist(device.id, item_names.tampering_move_alarm)
end
