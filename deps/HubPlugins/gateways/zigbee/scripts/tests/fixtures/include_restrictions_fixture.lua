local fixture = hubplugins.weak_loadfile("HUB:zigbee/scripts/tests/fixtures/base_fixture")()

local tests = require("tests")

function fixture.setup_restrictions(restrictions)
    local restrictions_helper = hubplugins.object("include_restrictions_helper", "HUB:zigbee/scripts/helpers")
    restrictions_helper:call("restriction_added", restrictions)
end

function fixture.reset_restrictions()
    local restrictions_helper = hubplugins.object("include_restrictions_helper", "HUB:zigbee/scripts/helpers")
    restrictions_helper:call("include_finished")
end

function fixture.check_restrictions(restrictions)
    local restrictions_helper = hubplugins.object("include_restrictions_helper", "HUB:zigbee/scripts/helpers")
    tests.assert_eq(restrictions_helper:call("storage/find_include_restrictions"), restrictions)
end

function fixture.check_node_restricted(node_id, value)
    local restrictions_helper = hubplugins.object("include_restrictions_helper", "HUB:zigbee/scripts/helpers")
    tests.assert_eq(restrictions_helper:call("storage/is_node_restricted", node_id), value)
end

function fixture.get_multisensor_node()
    local function cluster_ias_zone_generator(zone_type)
        return {
            name = "ias_zone",
            attributes = {
                {
                    name = "ZoneType",
                    formatted_value = {zone_type = zone_type}
                }
            }
        }
    end

    local endpoint_1 = {
        id = 1,
        clusters = { cluster_ias_zone_generator("motion_sensor") }
    }

    local endpoint_2 = {
        id = 2,
        clusters = { cluster_ias_zone_generator("fire_sensor") }
    }

    local endpoint_3 = {
        id = 3,
        clusters = { cluster_ias_zone_generator("gas_sensor") }
    }

    return {
        id = 9000,
        manufacturer_id = "Ezlo",
        model_id = "Combo Sensor",
        security = 0,
        is_reachable = 1,
        is_battery_powered = false,

        endpoints = { endpoint_1, endpoint_2, endpoint_3 }
    }
end

return fixture
