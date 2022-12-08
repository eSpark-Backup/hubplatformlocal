local item = hubplugins.weak_loadfile("HUB:zwave/scripts/model/items/default/test_state")()
local test_state_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/test_state")

hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/append_to_table")(
    item.enum,
    test_state_enum.no_test,
    test_state_enum.test_ok
)

return item
