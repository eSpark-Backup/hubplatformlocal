local params = ...

local tree_model = params.tree_model
local node = params.node

local function collect_items(node)
    local result = {}
    local callback = function(dev_node)
        local dev_data = tree_model:call("get_data", dev_node)

        for _, item in ipairs(dev_data.items) do
            result[item.name] = true
        end
    end
    tree_model:call("direct_walk", callback)
    return result
end


-- main
print("Detect device category by items")

local items = collect_items(node)

return hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/device_info_detection/items_based/ensure_door_lock")(items)
    or hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/device_info_detection/items_based/ensure_garrage_door")(items)
    or hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/device_info_detection/items_based/ensure_thermostat")(items)
    or hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/device_info_detection/items_based/ensure_doorbell")(items)
    or hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/device_info_detection/items_based/ensure_siren")(items)
    or hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/device_info_detection/items_based/ensure_window_covering")(items)
    or hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/device_info_detection/items_based/ensure_light_rgb")(items)
    or hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/device_info_detection/items_based/ensure_light_bulb")(items)
    or hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/device_info_detection/items_based/ensure_dimmable_plug")(items)
    or hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/device_info_detection/items_based/ensure_plug")(items)
    or hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/device_info_detection/items_based/ensure_flow_meter")(items)
    or hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/device_info_detection/items_based/ensure_hvac")(items)
    or hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/device_info_detection/items_based/ensure_scene_controller")(items)

