local params = ...

loadfile( "HUB:test_plugin/scripts/items_values_randomizer/randomize" )( 
    params.item_names, 
    params.item_ids, 
    params.timeout, 
    params.cyclicality 
)
