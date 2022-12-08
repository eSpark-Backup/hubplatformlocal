local tests = require("tests")
local log = require("logging")

print('Log level is set to: ', log.get_level())

assert(loadfile("HUB:ezlo_geofencing/scripts/create_abstract_subscription"))
assert(loadfile("HUB:ezlo_geofencing/scripts/update_abstract_subscription"))
assert(loadfile("HUB:ezlo_geofencing/scripts/delete_abstract_subscription"))
assert(loadfile("HUB:ezlo_geofencing/scripts/helpers/mapping_utils"))

