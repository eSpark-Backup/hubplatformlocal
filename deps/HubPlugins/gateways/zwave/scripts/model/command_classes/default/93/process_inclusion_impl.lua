local self, inclusion_context, build_tree_context, device_node = ...

if not hubplugins.device_settings_enabled then
    return
end

if build_tree_context.channel.id ~= 0 then
    print("Antitheft cc in " .. build_tree_context.channel.id .. " channel. How to handle it?")
    return
end

local cc = build_tree_context.command_class

local class_ids = require("HUB:zwave/scripts/resources/class_ids")

local device_node_data = inclusion_context.device_tree_model:call("get_data", device_node)

local function add_setting(setting_tag, value)
    local setting_info = self:call("settings/" .. setting_tag)

    local setting = {
        label = {
            text = setting_info.name
        },
        description = {
            text = setting_info.description
        },
        value_type = setting_info.value_type,
        value = value,
        value_default = setting_info.value_default,
        enum = setting_info.enum,
        descriptor = {
            node_id = inclusion_context.zwave_node.id,
            class_id = class_ids.anti_theft,
            channel_id = build_tree_context.channel.id,
            tag = setting_tag
        }
    }

    table.insert(device_node_data.settings, setting)
end

local own_manufacturer_id = 603

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
if cc.initial_lock_info then
    storage:call("set_string", storage:call("get_anti_theft_hint_key", inclusion_context.zwave_node.id), cc.initial_lock_info.hint)
else
    storage:call("remove_value", storage:call("get_anti_theft_hint_key", inclusion_context.zwave_node.id))
end

if cc.initial_lock_info and ( cc.initial_lock_info.restricted or cc.initial_lock_info.manufacturer_id ~= own_manufacturer_id ) then
    local zwave = require "zwave"

    add_setting("lock_info",
                {
                    hint = cc.initial_lock_info.hint,
                    manufacturer = zwave.get_manufacturer_name( cc.initial_lock_info.manufacturer_id ),
                    zwave_alliance_locking_id = cc.initial_lock_info.zwave_alliance_locking_id and 
                                                tostring(cc.initial_lock_info.zwave_alliance_locking_id) -- TODO: add locker description instead
                })
    add_setting("password", "")

    if cc.initial_lock_info.manufacturer_id == own_manufacturer_id then
        add_setting("auto_unlock", { text = "Auto Unlock" })
    end
else
    add_setting("anti_theft_enabled", cc.initial_lock_info ~= nil)
end

