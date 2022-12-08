local tests = require("tests")
local fixture = require("HUB:2gig/scripts/tests/migration/migration_fixture")
local item_names = require("HUB:2gig/scripts/enum/item_names")

local function create_device()
    local device = {
        name = "Panic Button",
        device_type = "panic_button",
        device_type_id = "panic_button",
        type = "controller.panic",
        category = "scene_controller",
        subcategory = "panic",
        battery_powered = true,
        reachable = true
    }

    function device.create_items()
        local item_factory = require("HUB:2gig/scripts/helpers/item_factory")

        return {
            item_factory.create_button_state_item(1, { "idle", "held_down" }),
            item_factory.create_battery_item(),
    }
    end

    return fixture.add_device(device, { 123 })
end

local device = create_device()

do
    tests.assert_eq(fixture.get_items_count(device.id), 2)

    fixture.check_item_doesnt_exist(device.id, item_names.security_threat)
end

fixture.migrate_to(5)

do
    tests.assert_eq(fixture.get_items_count(device.id), 3)

    fixture.check_item_exists(device.id, item_names.security_threat)
end
