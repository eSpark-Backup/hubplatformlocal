local params = ...

require("HUB:zwave/scripts/helpers/scripts_preload")

local core = require "core"
local item_names = require("HUB:zwave/scripts/resources/item_names")
local button_states = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/button_actions")

local dev_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_handler_monitoring")(params.node_id)
local item = core.get_item(params.item_id)

if item and item.value.button_state == button_states.idle then
    local button_number = item.value.button_number

    dev_handler:call(
        "update_value_in_core",
        item_names.button_state,
        params.item_id,
        {button_number = button_number, button_state = button_states.press_1_time}
    )

    dev_handler:call(
        "update_value_in_core",
        item_names.button_state,
        params.item_id,
        {button_number = button_number, button_state = button_states.idle}
    )
end
