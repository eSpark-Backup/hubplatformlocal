local tests = require("tests")

local zwave = {
    nodes = {},
}

function zwave.get_node( id )
    return tests.deep_copy(zwave.nodes[ tostring( id ) ])
end

function zwave.has_node(node_id)
    return zwave.nodes[ tostring( node_id ) ] and true or false
end

function zwave.get_node_info( node_id )
    local node = zwave.get_node( node_id )
    if node then
        local node_info = {}
        node_info.__index =
            function(_, key)
                if key ~= "channels" then
                    return node[ key ]
                end

                return nil
            end

        return setmetatable( node_info, node_info )
    end

    return nil
end

function zwave.get_command_class( node_id, channel_id, class_id )
    local node = zwave.nodes[ tostring( node_id ) ]
    if node then
        for _, channel in ipairs(node.channels) do
            if channel.id == channel_id then
                for _, cc in ipairs(channel.classes) do
                    if cc.id == class_id then
                        return tests.deep_copy(cc)
                    end
                end
            end
        end
    end
end

function zwave.add_node( node )
    zwave.nodes[ tostring( node.id ) ] = node
end

function zwave.remove_node( id )
    zwave.nodes[ tostring( id ) ] = nil
end

function zwave.get_all_nodes_ids()
    local result = {}
    for id,_ in pairs(zwave.nodes) do
        table.insert(result, id)
    end
    return result
end

function zwave.add_configuration(node_id, number, size)
end

function zwave.remove_configuration(node_id, number)
end

function zwave.get_info()
    return {
        module = "Z-Wave",
        module_firmware_version = "6.04",
        locale = "US",
        home_id = "123",
        node_id = 1,
        port = "/dev/stick",
        pole = {
            powered = true,
            battery = false
        },
        roles = {
            sis = true,
            primary = false,
            secondary = false,
            static = false,
            inclusion = false
        }
    }
end

function zwave.get_firmware_region()
    return "us"
end

function zwave.request_value( node_id, class_id, channel_id )
    error( "Please, override zwave.request_value" )
end

function zwave.get_manufacturer_name( manufacturer_id )
    return "manufacturer_" .. manufacturer_id
end

function zwave.request_node_values( node_id )
    error( "Please, override zwave.request_node_values" )
end

function zwave.set_value( node_id, class_id, channel_id )
    error( "Please, override zwave.set_value" )
end

function zwave.set_setpoint_value( node_id, class_id, channel_id, setpoint_type, value )
    error( "Please, override zwave.set_setpoint_value" )
end

function zwave.set_thermostat_mode( node_id, class_id, channel_id, mode, manufacturer_specific_data )
    error( "Please, override zwave.set_thermostat_mode" )
end

function zwave.set_auth_mode( modes )
    error( "Please, override zwave.set_auth_mode" )
end

function zwave.set_configuration_value( node_id, param_number, param_size, value )
    error( "Please, override zwave.set_configuration_value" )
end

function zwave.reset_configuration_value( node_id, param_number )
    error( "Please, override zwave.set_configuration_value" )
end

function zwave.set_week_day_entry_lock_schedule( node_id, channel_id, user_id, slot_id, value )
    error( "Please, override zwave.set_week_day_entry_lock_schedule" )
end

function zwave.request_week_day_entry_lock_schedule( node_id, channel_id, user_id, slot_id, value )
    error( "Please, override zwave.request_week_day_entry_lock_schedule" )
end

function zwave.set_year_day_entry_lock_schedule( node_id, channel_id, user_id, slot_id, value )
    error( "Please, override zwave.set_year_day_entry_lock_schedule" )
end

function zwave.request_year_day_entry_lock_schedule( node_id, channel_id, user_id, slot_id, value )
    error( "Please, override zwave.request_year_day_entry_lock_schedule" )
end

function zwave.set_daily_repeating_entry_lock_schedule( node_id, channel_id, user_id, slot_id, value )
    error( "Please, override zwave.set_daily_repeating_entry_lock_schedule" )
end

function zwave.request_daily_repeating_entry_lock_schedule( node_id, channel_id, user_id, slot_id, value )
    error( "Please, override zwave.request_daily_repeating_entry_lock_schedule" )
end

function zwave.set_user_code( node_id, channel_id, user_id, status, code )
    error( "Please, override zwave.set_user_code" )
end

function zwave.request_user_code( node_id, channel_id, user_id )
    error( "Please, override zwave.request_user_code" )
end

function zwave.set_master_code( node_id, channel_id, code )
    error( "Please, override zwave.set_master_code" )
end

function zwave.request_master_code( node_id, channel_id )
    error( "Please, override zwave.request_master_code" )
end

function zwave.set_user_codes_keypad_mode( node_id, channel_id, mode )
    error( "Please, override zwave.set_user_codes_keypad_mode" )
end

function zwave.request_user_codes_keypad_mode( node_id, channel_id )
    error( "Please, override zwave.request_user_codes_keypad_mode" )
end

function zwave.set_door_lock_configuration( node_id, channel_id, configuration )
    error( "Please, override zwave.set_door_lock_configuration" )
end

function zwave.request_door_lock_configuration( node_id, channel_id )
    error( "Please, override zwave.request_door_lock_configuration" )
end

function zwave.set_door_lock_mode( node_id, channel_id, mode )
    error( "Please, override zwave.set_door_lock_mode" )
end

function zwave.request_door_lock_state( node_id, channel_id )
    error( "Please, override zwave.request_door_lock_state" )
end

function zwave.update_nodes_firmware( node_ids, firmware_list )
    error( "Please, override zwave.update_nodes_firmware" )
end

function zwave.start_rediscover( node_id, device_settings_file )
    error( "Please, override zwave.start_rediscover" )
end

function zwave.lock_anti_theft_default( node_id, channel_id )
    error( "Please, override zwave.lock_anti_theft_default" )
end

function zwave.lock_anti_theft( node_id, channel_id, magic_code, hint )
    error( "Please, override zwave.lock_anti_theft" )
end

function zwave.unlock_anti_theft_default( node_id, channel_id, class_id, hint )
    error( "Please, override zwave.unlock_anti_theft_default" )
end

function zwave.unlock_anti_theft( node_id, channel_id, class_id, magic_code )
    error( "Please, override zwave.unlock_anti_theft" )
end

function zwave.set_default_device_configurations_file(file_path)
    print( "Ignore set_default_device_configurations_file method" )
end

return zwave
