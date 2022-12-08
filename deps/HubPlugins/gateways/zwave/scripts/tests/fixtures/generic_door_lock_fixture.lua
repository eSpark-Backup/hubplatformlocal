local fixture = {}

local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local zwave = require( "zwave" )
local core = require( "core" )
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")


-- static variables
fixture.node_id = 9000
fixture.channel_0_id = 0


function fixture:add_user_code(user_id, usercode)
    events_handler( {
        event = "user_code_updated",
        node_id = fixture.node_id,
        channel_id = fixture.channel_0_id,
        class_id = class_ids.user_codes,
        supported_users_count = 250,
        user_polling_status = "idle",
        user_code_ranges = {
            {
                user_id = user_id,
                user_status = "enabled",
                user_code = usercode
            }
        }
    } )
end


function fixture:get_item_value(item_name)
    return self:get_item(item_name).value
end


function fixture:get_item(item_name)
    return core.get_item(self:get_item_id(item_name))
end


function fixture:get_item_id(item_name)
    if item_name == "user_codes" then
        return storage:call("find_item_id", item_name, fixture.node_id, class_ids.user_codes, fixture.channel_0_id)
    else
        return storage:call("find_item_id", item_name, fixture.node_id, class_ids.notification, fixture.channel_0_id, "access_control")
    end
end


function fixture:send_notification( event_type, alarm_type, alarm_level, --[[opt]]value )
    events_handler( {
        event = "notification_recieved",
        node_id = fixture.node_id,
        channel_id = fixture.channel_0_id,
        class_id = class_ids.notification,
        v1_alarm_type = alarm_type,
        v1_alarm_level = alarm_level,
        notification_type = "access_control",
        event_type = event_type,
        value = value,
        raw_base64_value = nil,
    } )
end


function fixture:send_alarm(alarm_type, alarm_level)
    events_handler( {
        event = "unknown_notification_recieved",
        node_id = fixture.node_id,
        channel_id = fixture.channel_0_id,
        class_id = class_ids.notification,
        v1_alarm_type = alarm_type,
        v1_alarm_level = alarm_level,
        event_type = 0,
    } )
end


function fixture:create_and_add_zwave_node(device_info, cc_config)
    local zwave_node = self:create_node(device_info, cc_config)
    self:add_node(zwave_node)
    return zwave_node
end


function fixture:add_node(zwave_node)
    -- prepare zwave
    zwave.add_node(zwave_node)

    -- add node
    events_handler( {
        event = "node_added",
        node_id = tonumber(zwave_node.id)
    } )
end

function fixture:remove_node(id)
    id = id or fixture.node_id
    local node = zwave.get_node(id)

    zwave.remove_node(id)

    events_handler( {
        event = "node_removed",
        node_id = id,
        manufacturer_id = node.manufacturer_id,
        product_type = node.product_type,
        product_id = node.product_id
    } )
end

function fixture:create_node(device_info, cc_config)
    cc_config = cc_config or {
        has_schedule = true,
        notification_version = 8,
        alarm_v1_supported = true,
        has_power_management_notifications = true
    }

    local channel_0 = {
        id = fixture.channel_0_id,
        classes = {
            { id = 115 },
            {
                id = class_ids.configuration,
                version = 1
            },
            { id = class_ids.battery },
            { id = class_ids.door_lock },
            {
                id = class_ids.user_codes,
                supported_users_count = 250
            },
            { id = 122 },
        }
    }

    if cc_config.has_schedule then
        table.insert(channel_0.classes,
            {
                id = class_ids.schedule_entry_lock,
                year_day_slots = 0,
                week_day_slots = 0,
                daily_repeating_slots = 1
            })
    end

    if cc_config.notification_version then
        local notification_cc = {
            id = class_ids.notification,
            version = cc_config.notification_version,
            v1_supported = cc_config.alarm_v1_supported,
        }

        if cc_config.notification_version > 1 then
            notification_cc.notifications = {
                    {
                        type = "access_control",
                        event_types = { "manual_lock_operation",
                                       "manual_unlock_operation",
                                       "rf_lock_operation",
                                       "rf_unlock_operation",
                                       "keypad_lock_operation",
                                       "keypad_unlock_operation",
                                       "auto_lock_locked_operation",
                                       "lock_jammed",
                                       "all_user_codes_deleted",
                                       "single_user_code_deleted",
                                       "new_user_code_added",
                                       "new_user_code_not_added_due_to_duplicated_code",
                                       "keypad_temporary_disabled",
                                       "new_program_code_entered" }
                    }
                }

            if cc_config.has_power_management_notifications then
                table.insert(
                    notification_cc.notifications,
                    {
                        type = "power_management",
                        event_types = {
                            "power_has_been_applied",
                            "replace_battery_soon",
                            "replace_battery_now"
                        }
                    })
            end
        end

        table.insert(channel_0.classes, notification_cc)
    end

    local node = {
        id = tostring(fixture.node_id),
        manufacturer_id = device_info.manufacturer_id,
        product_type = device_info.product_type,
        product_id = device_info.product_id,
        is_battery_powered = true,

        specific_type = "SPECIFIC_TYPE_DOOR_LOCK",
        generic_type = "GENERIC_TYPE_ENTRY_CONTROL",

        security = { "s0" },
        security_class_started = { "s0" },

        channels = { channel_0 }
    }

    return node
end

function fixture:get_manufacturer_specifics()
    return {
        {
            value = {
                manufacturer_id = 297,
                product_type = 32770,
                product_id = 1536
            },
            description = "YRD226"
        },
        {
            value = {
                manufacturer_id = 265,
                product_type = 32770,
                product_id = 1536
            },
            description = "YRD/YRL 226 device with doorlock type auto detection"
        },
        {
            value = {
                manufacturer_id = 265,
                product_type = 2,
                product_id = 0
            },
            description = "YRD220-ZW"
        },
        {
            value = {
                manufacturer_id = 297,
                product_type = 2,
                product_id = 100500
            },
            description = "YRD220 with auto detection"
        },
        {
            value = {
                manufacturer_id = 297,
                product_id = 1489,
                product_type = 33028
            },
            description = "YRD410 with auto detection"
        },
        {
            value = {
                manufacturer_id = 144,
                product_type = 42,
                product_id = 100500
            },
            description = "Some kwikset with auto detection"
        }
    }
end


return fixture
