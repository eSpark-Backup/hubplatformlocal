local params = ...

local core = require "core"

print( "TEST_PLUGIN: Modify item" )
core.modify_item( params.item_id, params.item )
