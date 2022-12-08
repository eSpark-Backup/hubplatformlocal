local fixture = require("HUB:zwave/scripts/tests/fixtures/settings_fixture")

fixture.add_node()

fixture.check_device_settings({
    fixture.rediscovering_configuration,
    fixture.generic_zwave_configurations
})

fixture.remove_node()
