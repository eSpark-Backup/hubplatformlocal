local class_ids = require("HUB:zwave/scripts/resources/class_ids")

hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/multichannel_switch_fixture")(class_ids.switch_binary, {
    manufacturer_id = 99,
    product_type = 20562,
    product_id = 12594,
})
