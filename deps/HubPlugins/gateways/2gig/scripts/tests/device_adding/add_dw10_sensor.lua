local tests = require("tests")
local twogig = require("2gig")

local test_utils = require("HUB:2gig/scripts/tests/test_utils")
local item_factory = require("HUB:2gig/scripts/helpers/item_factory")
local setting_factory = require("HUB:2gig/scripts/helpers/setting_factory")
local add_device = loadfile("HUB:2gig/scripts/add_device")

add_device({
    model = "2GIG-DW10-345",
    serial = 123
})

tests.assert_called(twogig.configure_device, "Check twogig.configure_device call", {})

local device_expectation = {
    name = "Door & Window Sensor",
    category = "security_sensor",
    subcategory = "door",
    device_type_id = "2GIG-DW10-345",
    info = { manufacturer = "2GIG", model = "DW10-345" },
    battery_powered = true,
    items = {
        item_factory.create_door_window_state_item(),
        item_factory.create_security_threat_item(),
        item_factory.create_tampering_cover_alarm_item(),
        item_factory.create_battery_item()
    },
    settings = {
        setting_factory.create_token_setting(
            {
                tag = "dw_detection_source",
                label = "Sensing contact selection",
                description = "Select sinfle contact or mixed contact rule to detect sensor state",
                type = "token",
                enum = {
                    magnet_contact = "Use only magnet contact",
                    external_contact = "Use only external dry contact",
                    mixed_optimistic = "Use both contacts, door or window will remain closed if any contact closed",
                    mixed_pessimistic = "Use both contacts, door or window will remain opened if any contact opened"
                },
                default = "magnet_contact",
            }
        )
    }
}

test_utils.check_devices({ device_expectation })
