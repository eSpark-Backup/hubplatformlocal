local test = require( "tests" )

local security_field_helper = loadfile("HUB:zwave/scripts/helpers/get_node_security")

local zwave_security_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/zwave_security_flag")
local security_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/security_flag")

local s2_id = require("HUB:zwave/scripts/resources/class_ids").s2
local function create_node(security_class_started, security_class, has_s2)
    local s2_cc = has_s2 and { id = s2_id } or nil

    return {
        security_class_started = { security_class_started }, 
        security = { security_class },
        channels = {
            {
                id = 0,
                classes = { s2_cc }
            },
            {
                id = 1,
                classes = { }
            }
        }
    }
end

test.assert_eq(security_field_helper(create_node(zwave_security_enum.access_control, zwave_security_enum.access_control, true)), security_enum.high)
test.assert_eq(security_field_helper(create_node(zwave_security_enum.access_control, zwave_security_enum.unauthenticated, true)), security_enum.low)
test.assert_eq(security_field_helper(create_node(zwave_security_enum.access_control, zwave_security_enum.s0, true)), security_enum.low)
test.assert_eq(security_field_helper(create_node(zwave_security_enum.access_control, zwave_security_enum.none, true)), security_enum.no)

test.assert_eq(security_field_helper(create_node(zwave_security_enum.authenticated, zwave_security_enum.authenticated, true)), security_enum.high)
test.assert_eq(security_field_helper(create_node(zwave_security_enum.authenticated, zwave_security_enum.unauthenticated, true)), security_enum.low)
test.assert_eq(security_field_helper(create_node(zwave_security_enum.authenticated, zwave_security_enum.s0, true)), security_enum.low)
test.assert_eq(security_field_helper(create_node(zwave_security_enum.authenticated, zwave_security_enum.none, true)), security_enum.no)

test.assert_eq(security_field_helper(create_node(zwave_security_enum.unauthenticated, zwave_security_enum.unauthenticated, true)), security_enum.middle)
test.assert_eq(security_field_helper(create_node(zwave_security_enum.unauthenticated, zwave_security_enum.s0, true)), security_enum.low)
test.assert_eq(security_field_helper(create_node(zwave_security_enum.unauthenticated, zwave_security_enum.none, true)), security_enum.no)

test.assert_eq(security_field_helper(create_node(zwave_security_enum.s0, zwave_security_enum.s0)), security_enum.low)
test.assert_eq(security_field_helper(create_node(zwave_security_enum.s0, zwave_security_enum.s0, true)), security_enum.no)
test.assert_eq(security_field_helper(create_node(zwave_security_enum.s0, zwave_security_enum.none)), security_enum.no)

test.assert_eq(security_field_helper(create_node(zwave_security_enum.none, zwave_security_enum.none)), security_enum.no)
test.assert_eq(security_field_helper(create_node(zwave_security_enum.none, zwave_security_enum.none, true)), security_enum.no)