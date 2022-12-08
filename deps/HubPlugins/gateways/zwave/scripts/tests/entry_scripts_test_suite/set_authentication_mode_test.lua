local tests = require( "tests" )
local zwave = require( "zwave" )

zwave.set_auth_mode = tests.create_mock()

local set_authentication_mode = loadfile( "HUB:zwave/scripts/set_authentication_mode" )

set_authentication_mode( { modes = { "unauthenticated", "s0" } } )
tests.assert_called( zwave.set_auth_mode, "check passing several modes", { { { "unauthenticated", "s0" } } } )

set_authentication_mode( { modes = {} } )
tests.assert_called( zwave.set_auth_mode, "check passing empty modes", { { {} } } )

set_authentication_mode( { modes = nil } )
tests.assert_called( zwave.set_auth_mode, "check passing nil modes", { { {} } } )