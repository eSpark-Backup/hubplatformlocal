local tests = require("tests")
local fixture = require("HUB:2gig/scripts/tests/migration/migration_fixture")

local function create_device()
    local device = {
        name = "Keypad",
        device_type = "device",
        device_type_id = "keypad",
        type = "controller.remote",
        category = "keypad",
        subcategory = "",
        battery_powered = true,
        reachable = true,
    }

    function device.create_items()
        local item_factory = require("HUB:2gig/scripts/helpers/item_factory")

        return {
            item_factory.create_button_state_item(1, { "idle", "held_down" }),
            item_factory.create_button_state_item(2, { "idle", "held_down" }),
            item_factory.create_security_threat_item(),
            item_factory.create_battery_item()
        }
    end

    return fixture.add_device(device, { 123 })
end

local device = create_device()

do
    tests.assert_eq(fixture.get_settings_count(device.id), 0)

    fixture.check_setting_doesnt_exist(device.id, "pin_length")
    fixture.check_setting_doesnt_exist(device.id, "pin_input_timeout")
end

fixture.migrate_to(7)

do
    tests.assert_eq(fixture.get_settings_count(device.id), 2)

    fixture.check_setting_exists(device.id, "pin_length")
    fixture.check_setting_exists(device.id, "pin_input_timeout")
end
