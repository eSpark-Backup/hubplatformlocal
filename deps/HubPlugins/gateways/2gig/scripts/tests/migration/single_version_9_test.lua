local tests = require("tests")
local fixture = require("HUB:2gig/scripts/tests/migration/migration_fixture")
local item_names = require("HUB:2gig/scripts/enum/item_names")

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
            item_factory.create_battery_item(),
            item_factory.create_user_codes_item(),
            item_factory.create_user_code_action_item()
        }
    end

    return fixture.add_device(device, { 123 })
end

local device = create_device()

do
    tests.assert_eq(fixture.get_items_count(device.id), 6)

    fixture.check_item_doesnt_exist(device.id, item_names.weekly_user_code_intervals)
    fixture.check_item_doesnt_exist(device.id, item_names.daily_user_code_intervals)
end

fixture.migrate_to(9)

do
    tests.assert_eq(fixture.get_items_count(device.id), 8)

    fixture.check_item_exists(device.id, item_names.weekly_user_code_intervals)
    fixture.check_item_exists(device.id, item_names.daily_user_code_intervals)
end
