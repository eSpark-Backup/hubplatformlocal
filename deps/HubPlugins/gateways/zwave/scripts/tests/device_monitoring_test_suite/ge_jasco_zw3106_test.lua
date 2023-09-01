local class_ids = require("HUB:zwave/scripts/resources/class_ids")

hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/multichannel_switch_fixture")(class_ids.dimmer, {
    manufacturer_id = 99,
    product_type = 20548,
    product_id = 12594,
})
