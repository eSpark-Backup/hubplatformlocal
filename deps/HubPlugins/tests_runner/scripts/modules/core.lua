local tests = require("tests")

local core = {
    gateway = {
        id = "gateway",
        manual_device_adding = "wizard"
    },

    devices = {},
    items = {},
    settings = {},
    gateway_settings = {},
    plugin_id = "plugin_id"
}

local device_index = 1
local item_index = 1
local setting_index = 1

function core.add_item( new_item )
    assert( new_item )
    local id = "item_id_" .. item_index
    item_index = item_index + 1
    local item = tests.deep_copy(new_item)
    local item_tag = item.descriptor and item.descriptor.tag or item.tag

    item.id = id
    item.descriptor = nil

    core.items[ id ] = item


    print( ("core.add_item: id: %s, name: %s, tag: %s, device_id: %s"):format( id, item.name, item_tag, item.device_id ) )

    return id
end

function core.get_item( id )
    return tests.deep_copy(core.items[ id ])
end

function core.get_device( id )
    return tests.deep_copy(core.devices[id])
end

function core.get_root_device_id( id )
    local device = core.devices[id]
    while device.parent_device_id ~= "" do
        device = core.devices[device.parent_device_id]
    end

    return device.id
end

function core.add_device( new_device )
    assert( new_device )
    local id = "device_id_" .. device_index
    device_index = device_index + 1
    local device = tests.deep_copy(new_device)
    device.id = id
    device.gateway_id = core.get_gateway().id
    if device.ready == nil then
        device.ready = true
    end
    if not device.parent_device_id then
        device.parent_device_id = ""
    end

    core.devices[ id ] = device

    print( ( "core.add_device: id: %s, parent_device_id: %s, name: %s, category: %s, subcategory: %s" ):format(
            id, tostring( device.parent_device_id ), device.name, device.category, tostring( device.subcategory ) ) )

    return id
end

function core.remove_device( device_id )
    assert( core.devices[ device_id ] )
    print("core.remove_device: id: " .. device_id)

    local children = {}
    local items = core.get_items_by_device_id( device_id )
    local setting_ids = core.get_setting_ids_by_device_id( device_id )

    core.devices[ device_id ] = nil

    for id, device in pairs( core.devices ) do
        if device.parent_device_id == device_id then
            table.insert( children, id )
        end
    end

    if items then
        for _, item in pairs( items ) do
            core.items[ item.id ] = nil
        end
    end

    if setting_ids then
        for _, setting_id in pairs( setting_ids ) do
            core.settings[ setting_id ] = nil
        end
    end

    for _, id in ipairs( children ) do
        core.remove_device( id )
    end
end

function core.remove_device_sync( device_id )
    core.remove_device( device_id )
end

function core.remove_item( item_id )
    print("core.remove_item: id: " .. item_id)

    core.items[ item_id ] = nil
end

function core.get_items_by_device_id( device_id )
    assert( core.devices[ device_id ] )

    local items = {}

    for _, item in pairs( core.items ) do
        if item.device_id == device_id then
            table.insert( items, tests.deep_copy(item) )
        end
    end

    return next(items) and items or nil
end

function core.get_devices()
    local result = {}
    for _, device in pairs( core.devices ) do
        table.insert( result, tests.deep_copy(device) )
    end
    return #result ~= 0 and result or nil
end

function core.get_items()
    local result = {}
    for _, item in pairs( core.items ) do
        table.insert( result, tests.deep_copy(item) )
    end
    return result
end

function core.get_gateway()
    return tests.deep_copy(core.gateway)
end

function core.update_gateway(fields)
    for key, value in pairs(fields) do
        core.gateway[key] = tests.deep_copy(value)
    end
end

function core.update_user_code_restriction( item_id, restriction )
    assert( item_id ~= nil, "item id is nil" )
    assert( restriction ~= nil, "value is nil" )

    local item = assert( core.items[ item_id ], "item not found" )
    item.user_code_restriction = restriction
end

function core.update_item_value( item_id, value )
    assert( item_id ~= nil, "item id is nil" )
    assert( value ~= nil, "value is nil" )

    local item = assert( core.items[ item_id ], "item not found" )
    item.value = value
end

function core.update_item_value_with_min_max( item_id, value, min, max )
    core.update_item_value( item_id, value )
end

function core.modify_device(device_id, new_device)
    assert( device_id ~= nil, "device id is nil" )
    assert( new_device ~= nil, "device is nil" )
    local device = assert( core.devices[device_id], "device not found" )

    print("core.modify_device " .. device_id)

    device.category = new_device.category
    device.subcategory = new_device.subcategory
    device.type = new_device.type
    device.security = new_device.security
    device.name = new_device.name
    device.parent_device_id = new_device.parent_device_id
    device.info = new_device.info
    device.firmware = new_device.firmware
    device.house_modes_options = new_device.house_modes_options
    device.device_type_id = new_device.device_type_id
    device.battery_powered = new_device.battery_powered
end

function core.update_reachable_state(device_id, reachable)
    assert(device_id ~= nil, "device_id id is nil")
    assert(reachable ~= nil, "reachable is nil")

    local device = assert(core.devices[device_id], "device not found")

    device.reachable = reachable
end

function core.update_ready_state(device_id, ready)
    assert(device_id ~= nil, "device_id id is nil")
    assert(ready ~= nil, "ready is nil")

    local device = assert(core.devices[device_id], "device not found")

    device.ready = ready
end

function core.modify_item(item_id, new_item)
    assert( item_id ~= nil, "item id is nil" )
    assert( new_item ~= nil, "item is nil" )

    print("core.modify_item " .. item_id)

    local item = assert( core.items[ item_id ], "item not found" )

    for key, value in pairs(item) do
        if new_item[key] ~= nil and new_item[key] ~= value then
            item[key] = new_item[key]
        end
    end
end

function core.modify_setting(setting_id, new_setting)
    assert( setting_id ~= nil, "setting id is nil" )
    assert( new_setting ~= nil, "setting is nil" )

    print("core.modify_setting " .. setting_id)

    local setting = assert( core.settings[setting_id], "setting not found" )

    for key, value in pairs(setting) do
        if new_setting[key] and new_setting[key] ~= value then
            setting[key] = new_setting[key]
        end
    end
end

function core.remove_item_dictionary_value(item_id, key)
    assert( item_id ~= nil, "item id is nil" )
    assert( key ~= nil, "key is nil" )

    local item = assert( core.items[ item_id ], "item not found" )

    if item.value then
        item.value[ key ] = nil
    end
end

function core.update_item_dictionary_value(item_id, key, value)
    assert( item_id ~= nil, "item id is nil" )
    assert( key ~= nil, "key is nil" )
    assert( value ~= nil, "value is nil" )

    local item = assert( core.items[ item_id ], "item not found" )

    item.value = item.value or {}
    item.value[ key ] = tests.deep_copy(value)
end

function core.get_item_dictionary_value(item_id, key)
    assert( item_id ~= nil, "item id is nil" )
    assert( key ~= nil, "key is nil" )

    local item = assert( core.items[ item_id ], "item not found" )

    return item.value and tests.deep_copy(item.value[key])
end

function core.update_device_status( device_id, status )
    assert(type ( device_id ) == "string", "device_id must be string" )
    assert(type ( status ) == "string", "status must be string" )

    local device = assert( core.devices[device_id], "device not found" )

    device.status = status
end

function core.add_setting(new_setting)
    assert(new_setting)

    local id = "setting_id_" .. setting_index
    setting_index = setting_index + 1

    local setting = tests.deep_copy(new_setting)
    setmetatable(setting, {
        __index = {
            id = id,
            _index = setting_index - 1, -- Used for ordering
            status = "pending"
        }
    })

    local tag = setting.descriptor and setting.descriptor.tag or setting.tag

    setting.descriptor = nil
    setting.value_mapping = nil

    core.settings[id] = setting

    print(("core.add_setting: id: %s, type: %s, device_id: %s, tag: %s"):format(id, setting.value_type, setting.device_id, tag))

    return id
end

function core.get_settings()
    local settings = {}
    for _, setting in pairs(core.settings) do
        settings[#settings + 1] = tests.deep_copy(setting)
    end

    table.sort(settings, function(l, r)
        return l._index < r._index
    end)

    return settings
end

function core.get_setting(id)
    return tests.deep_copy(core.settings[id])
end

function core.get_setting_ids_by_device_id(device_id)
    local settings = {}
    for _, setting in pairs(core.settings) do
        if setting.device_id == device_id then
            settings[#settings + 1] = setting
        end
    end

    if #settings == 0 then return end

    table.sort(settings, function(l, r)
        return l._index < r._index
    end)

    for idx, setting in pairs(settings) do
        settings[idx] = setting.id
    end

    return settings
end

function core.set_setting_value(id, value, status)
    local setting = assert(core.settings[id], "setting not found")
    setting.value = value
    if status then
        core.update_setting_status( id, status )
    end
end

function core.update_setting_status( id, status )
    local setting = assert(core.settings[ id ], "setting not found")
    assert(status == "synced" or status == "failed" or status == "pending")
    getmetatable(setting).__index.status = status
end

function core.set_setting_dictionary_value(id, key, value)
    local setting = assert(core.settings[id], "setting not found")
    setting.value[key] = tests.deep_copy(value)
end

function core.remove_setting(id)
    print("core.remove_setting: " .. id)
    core.settings[id] = nil
end

function core.send_ui_broadcast( params )
    print( "Please, override core.send_ui_broadcast" )
end

function core.send_response( operation_id, result_table, error_table )
    error( "Please, override core.send_response" )
end

function core.send_device_firmware_download_broadcast( broadcast_info )
    error( "Please, override core.send_device_firmware_download_broadcast" )
end

function core.send_device_firmware_update_broadcast( broadcast_info )
    error( "Please, override core.send_device_firmware_update_broadcast" )
end

function core.subscribe( path )
    error( "Please, override core.subscribe" )
end

function core.unsubscribe( path )
    error( "Please, override core.unsubscribe" )
end

function core.set_plugin_id(id)
    core.plugin_id = id
end

function core.add_gateway_setting( new_setting )
    local setting = tests.deep_copy(new_setting)
    setting.id = core.plugin_id .. "." .. setting.id
    core.gateway_settings[ setting.id ] = setting

    print(("core.add_gateway_setting: id: %s").format(setting.id))
end

function core.remove_gateway_setting( setting_id )
    core.gateway_settings[setting_id] = nil
end

function core.set_gateway_setting_value( setting_id, value )
    core.gateway_settings[setting_id].value = tests.deep_copy(value)
end

function core.get_gateway_setting_ids_by_gateway_id( gateway_id )
    local result = {}
    for _, setting in pairs( core.gateway_settings ) do
        table.insert( result, tests.deep_copy(setting) )
    end
    return result
end

function core.get_gateway_setting( setting_id )
    return tests.deep_copy(core.gateway_settings[setting_id])
end

function core.notify_process_started( process )
end

function core.notify_process_stopped( process, result )
end

function core.get_current_notified_process()
    return nil
end

return core
