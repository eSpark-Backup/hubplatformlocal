-- @{ Small test for the logging module
--   TODO: Will be removed in future
--   TODO: Uncomment next code for test functionality of module
--

print( "TEST_PLUGIN: ---> Test of logging" )

local logging = require "logging"

local logLevelValue = logging.get_level()
print( "logLevelValue = ", logLevelValue )

logging.log("--fake--", "test string data...[log -- with 'INFO' <default value>]")
logLevelValue = logging.get_level()
print( "logLevelValue = ", logLevelValue )

logging.log("error", "test string data...[log -- with 'ERROR']")
logLevelValue = logging.get_level()
print( "logLevelValue = ", logLevelValue )

logging.trace("test string data...[trace]")
logLevelValue = logging.get_level()
print( "logLevelValue = ", logLevelValue )

logging.debug("test string data...[debug]")
logLevelValue = logging.get_level()
print( "logLevelValue = ", logLevelValue )

logging.info("test string data...[info]")
logLevelValue = logging.get_level()
print( "logLevelValue = ", logLevelValue )

logging.warning("test string data...[warning]")
logLevelValue = logging.get_level()
print( "logLevelValue = ", logLevelValue )

logging.error("test string data...[error]")
logLevelValue = logging.get_level()
print( "logLevelValue = ", logLevelValue )

print( "TEST_PLUGIN: <--- Test of logging" )

--

-- @}