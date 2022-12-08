local fixture = loadfile( "HUB:zwave/scripts/tests/gateway_settings_test_suite/fixtures/base_fixture" )()
fixture.init()

fixture.set_zwave_info_field("port", "/dev/stick")

-- emulate plugin starting
loadfile( "HUB:zwave/scripts/startup" )()

local expected = fixture.get_mandatory_settings_expectation()
table.insert(expected,
    {
        id = "port",
        label = {
            text = "Port",
            lang_tag = "gateway_setting_port"
        },
        status = "synced",
        value_type = "string",
        value = "/dev/stick",
        has_setter = false
    }
)

fixture.check_gateway_settings(expected)

fixture.zwave_info = {
    module = "Z-Wave",
    module_firmware_version = "6.06",
    locale = "EU",
    home_id = "321",
    node_id = 2,
    port = "/dev/stick_new",
    roles = {
        sis = false,
        primary = false,
        secondary = true,
        static = false,
        inclusion = false
    }
}

loadfile( "HUB:zwave/scripts/startup" )()

expected[7].value = "321"
expected[8].value = "EU"
expected[9].value = "2"
expected[10].value = "Suc SIS: NO PRI: NO"
expected[11].value = "6.06"
expected[13].value = "/dev/stick_new"

fixture.check_gateway_settings(expected)
