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
