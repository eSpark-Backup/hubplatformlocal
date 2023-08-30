local params = ...

loadfile("HUB:2gig/scripts/model/monitoring/set_item_dictionary_value")(
    params.item_id, 
    params.key, 
    params.value
)
