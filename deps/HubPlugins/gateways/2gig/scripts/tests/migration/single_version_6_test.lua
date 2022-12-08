local tests = require("tests")
local fixture = require("HUB:2gig/scripts/tests/migration/migration_fixture")
local storage_utils = require("HUB:2gig/scripts/helpers/storage_utils")
local item_names = require("HUB:2gig/scripts/enum/item_names")

local function create_device()
    local smoke_detector = {
        name = "Audio Detector",
        device_type = "smoke_co_detector",
        device_type_id = "audio_detector",
        type = "sensor.multisensor",
        category = "security_sensor",
        subcategory = "smoke",
        battery_powered = true,
        reachable = true
    }

    function smoke_detector.create_items()
        local item_factory = require("HUB:2gig/scripts/helpers/item_factory")

        return {
            item_factory.create_smoke_alarm_item(),
            item_factory.create_security_threat_item(),
            item_factory.create_tampering_cover_alarm_item(),
            item_factory.create_battery_item(),
        }
    end

    function smoke_detector.get_item_ids(serial)
        local storage_utils = require("HUB:2gig/scripts/helpers/storage_utils")
        local item_names = require("HUB:2gig/scripts/enum/item_names")

        local core_device_id = assert(storage_utils.find_device_id(serial), "Device not found")

        return {
            smoke_alarm_id           = storage_utils.find_item_id(core_device_id, item_names.smoke_alarm),
            co_alarm_id              = storage_utils.find_item_id(core_device_id, item_names.co_alarm),
            security_threat_id       = storage_utils.find_item_id(core_device_id, item_names.security_threat),
            tampering_cover_alarm_id = storage_utils.find_item_id(core_device_id, item_names.tampering_cover_alarm),
            battery_id               = storage_utils.find_item_id(core_device_id, item_names.battery)
        }
    end

    local co_detector = loadfile("HUB:2gig/scripts/helpers/make_child_device")(smoke_detector)

    function co_detector.create_items()
        local item_factory = require("HUB:2gig/scripts/helpers/item_factory")

        return {
            item_factory.create_co_alarm_item()
        }
    end

    return fixture.add_device(smoke_detector, { 123, 124 })
end

local smoke_detector, co_detector = create_device()

do
    tests.assert_eq(fixture.get_items_count(smoke_detector.id), 4)
    tests.assert_eq(fixture.get_items_count(co_detector.id), 1)

    fixture.check_item_doesnt_exist(co_detector.id, item_names.security_threat)
end

fixture.migrate_to(6)

do
    tests.assert_eq(fixture.get_items_count(smoke_detector.id), 4)
    tests.assert_eq(fixture.get_items_count(co_detector.id), 2)

    fixture.check_item_exists(co_detector.id, item_names.security_threat)
end
