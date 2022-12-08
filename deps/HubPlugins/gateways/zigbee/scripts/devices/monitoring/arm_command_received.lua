local handler, node, endpoint_id, cluster_name, arm_mode, pin_code = ...

local zigbee = require("zigbee")
local house_modes = require("house_modes")

local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")
local user_code_actions = hubplugins.weak_require("HUB:zigbee/scripts/resources/enums/user_code_actions")
local zone_status_enum = hubplugins.weak_loadfile("HUB:zigbee/scripts/resources/enums/zone_status")()

local function get_pin_code_id(dictionary, pin_code)
    for key, value in pairs(dictionary) do
        if value.code == pin_code then
            return key
        end
    end
end

local function process_user_code()
    local user_codes_item = handler:find_item(
        item_names.user_codes,
        node.id,
        endpoint_id,
        cluster_name,
        "" )
    assert(user_codes_item, "No user_codes item")

    local user_code_action_item_id = handler:find_item_id(
        item_names.user_code_action,
        node.id,
        endpoint_id,
        cluster_name,
        "" )
    assert(user_code_action_item_id, "No user_code_action item")

    local pin_code_id = get_pin_code_id(user_codes_item.value, pin_code)

    local correct_pin_code = false
    if pin_code_id then
        handler:update_value_in_core(
            user_code_action_item_id,
            {
                userId = tonumber(pin_code_id),
                action = user_code_actions.user_code_entered
            })
        correct_pin_code = true
    else
        handler:update_value_in_core(
            user_code_action_item_id,
            {
                userId = -1,
                action = user_code_actions.invalid_user_code_entered
            })
    end

    handler:update_value_in_core(
        user_code_action_item_id,
        {
            userId = -1,
            action = user_code_actions.unknown
        })

    return correct_pin_code
end


-- main


local values_converters = hubplugins.weak_loadfile("HUB:zigbee/scripts/resources/values_converters/ias_ace")()
local correct_pin_code = process_user_code()

if correct_pin_code then
    local zone_status_item = handler:find_item(
        item_names.zones_status,
        node.id,
        endpoint_id,
        cluster_name,
        "" )
    assert( zone_status_item )

    local old_zone_status = zone_status_item.value
    local new_zone_status = values_converters.arm_mode_to_zone_status[arm_mode]

    if new_zone_status then
        handler:update_value_in_core( zone_status_item.id, new_zone_status)

        if old_zone_status == "disarmed" and new_zone_status == "disarmed" then
            zigbee.send_command( node.id, endpoint_id, cluster_name, "ArmResponse", "already_disarmed" )
        else
            zigbee.send_command(
                node.id,
                endpoint_id,
                cluster_name,
                "ArmResponse",
                values_converters.zone_status_to_arm_response[new_zone_status]
            )
        end

        -- Change house mode
        house_modes.cancel_switch()
        local new_house_mode = values_converters.arm_mode_to_house_mode[arm_mode]
        local current_house_mode = house_modes.get_current()
        if new_house_mode ~= current_house_mode then
            house_modes.switch(new_house_mode)
        else
            zigbee.send_command(
                node.id,
                endpoint_id,
                cluster_name,
                "PanelStatusChanged",
                {panel_status = values_converters.zone_status_to_panel_status[new_zone_status]}
            )
        end
    end
else
    zigbee.send_command(
        node.id,
        endpoint_id,
        cluster_name,
        "ArmResponse",
        "invalid_arm_disarm_code"
     )
end
