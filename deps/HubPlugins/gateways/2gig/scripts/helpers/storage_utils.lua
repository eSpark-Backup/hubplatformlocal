local storage = require("storage")

function table:remove_if(predicate)
    local new_table = {}

    for _, value in ipairs(self) do
        if not predicate(value) then
            table.insert(new_table, value)
        end
    end

    return new_table
end

function table.shallow_compare(left, right)
    if #left ~= #right then
        return false
    end

    for idx, value in ipairs(left) do
        if right[idx] ~= value then
            return false
        end
    end

    return true
end

local function create_storage_key(...)
    local tokens = { ... }

    return table.concat(tokens, "/")
end

local function device_id_by_serial(serial)
    return create_storage_key("device_id_by_serial", serial)
end

local function serial_by_device_id(device_id)
    return create_storage_key("serial_by_device_id", device_id)
end

local function item_id_by_lookup_key(device_id, item_name, tag)
    return create_storage_key("item_id_by_lookup_key", device_id, item_name, tag)
end

local function item_lookup_keys_by_device_id(device_id)
    return create_storage_key("item_lookup_keys_by_device_id", device_id)
end

local function setting_id_by_lookup_key(device_id, tag)
    return create_storage_key("setting_id_by_lookup_key", device_id, tag)
end

local function setting_lookup_keys_by_device_id(device_id)
    return create_storage_key("setting_lookup_keys_by_device_id", device_id)
end

local storage_utils = {}

function storage_utils.register_device(serial, device_id)
    storage.set_string(device_id_by_serial(serial), device_id)
    storage.set_string(serial_by_device_id(device_id), serial)
end

function storage_utils.unregister_device(serial, device_id)
    storage.delete(device_id_by_serial(serial))
    storage.delete(serial_by_device_id(device_id))
end

function storage_utils.find_device_id(serial)
    return storage.get_string(device_id_by_serial(serial))
end

function storage_utils.find_serial(device_id)
    return storage.get_string(serial_by_device_id(device_id))
end

function storage_utils.register_items(device_id, items)
    storage_utils.register_entities(item_lookup_keys_by_device_id, item_id_by_lookup_key, device_id, items)
end

function storage_utils.register_item(device_id, item_id, item_name, item_tag)
    storage_utils.register_entity(item_lookup_keys_by_device_id, item_id_by_lookup_key, device_id, item_id, item_name, item_tag)
end

function storage_utils.unregister_items(device_id)
    storage_utils.unregister_entities(item_lookup_keys_by_device_id, item_id_by_lookup_key, device_id)
end

function storage_utils.unregister_item(device_id, item_name, item_tag)
    storage_utils.unregister_entity(item_lookup_keys_by_device_id, item_id_by_lookup_key, device_id, item_name, item_tag)
end

function storage_utils.register_settings(device_id, settings)
    storage_utils.register_entities(setting_lookup_keys_by_device_id, setting_id_by_lookup_key, device_id, settings)
end

function storage_utils.register_setting(device_id, setting_id, setting_tag)
    storage_utils.register_entity(setting_lookup_keys_by_device_id, setting_id_by_lookup_key, device_id, setting_id, setting_tag)
end

function storage_utils.unregister_settings(device_id)
    storage_utils.unregister_entities(setting_lookup_keys_by_device_id, setting_id_by_lookup_key, device_id)
end

function storage_utils.unregister_setting(device_id, setting_tag)
    storage_utils.unregister_entity(setting_lookup_keys_by_device_id, setting_id_by_lookup_key, device_id, setting_tag)
end

function storage_utils.register_entities(entity_lookup_keys_by_device_id, entity_id_by_lookup_key, device_id, entities)
    local lookup_keys = {}

    for core_id, lookup_key in pairs(entities) do
        storage.set_string(entity_id_by_lookup_key(device_id, table.unpack(lookup_key)), core_id)
        table.insert(lookup_keys, lookup_key)
    end

    storage.set_table(entity_lookup_keys_by_device_id(device_id), lookup_keys)
end

function storage_utils.register_entity(entity_lookup_keys_by_device_id, entity_id_by_lookup_key, device_id, entity_id, ...)
    local lookup_keys = storage.get_table(entity_lookup_keys_by_device_id(device_id))

    if lookup_keys then
        storage.set_string(entity_id_by_lookup_key(device_id, ...), entity_id)
        table.insert(lookup_keys, {...})

        storage.set_table(entity_lookup_keys_by_device_id(device_id), lookup_keys)
    end
end

function storage_utils.unregister_entities(entity_lookup_keys_by_device_id, entity_id_by_lookup_key, device_id)
    local lookup_keys = storage.get_table(entity_lookup_keys_by_device_id(device_id))

    if lookup_keys then
        for _, lookup_key in ipairs(lookup_keys) do
            storage.delete(entity_id_by_lookup_key(device_id, table.unpack(lookup_key)))
        end

        storage.delete(entity_lookup_keys_by_device_id(device_id))
    end
end

function storage_utils.unregister_entity(entity_lookup_keys_by_device_id, entity_id_by_lookup_key, device_id, ...)
    local old_lookup_key = {...}
    local lookup_keys = storage.get_table(entity_lookup_keys_by_device_id(device_id))

    if lookup_keys then
        storage.delete(entity_id_by_lookup_key(device_id, ...))

        local new_lookup_keys = table.remove_if(lookup_keys, function(lookup_key)
            return table.shallow_compare(lookup_key, old_lookup_key)
        end)

        storage.set_table(entity_lookup_keys_by_device_id(device_id), new_lookup_keys)
    end
end

function storage_utils.set_nearest_restriction_event_time(time)
    storage.set_table("2gig_pin_code_restriction_nearest_event_time", time)
end

function storage_utils.clear_nearest_restriction_event_time()
    storage.delete("2gig_pin_code_restriction_nearest_event_time")
end

function storage_utils.set_house_mode_to_change(mode_name)
    storage.set_string("2gig_house_mode_to_change", mode_name)
end

function storage_utils.clear_house_mode_to_change()
    storage.delete("2gig_house_mode_to_change")
end

function storage_utils.find_item_id(device_id, item_name, tag)
    return storage.get_string(item_id_by_lookup_key(device_id, item_name, tag))
end

function storage_utils.find_setting_id(device_id, tag)
    return storage.get_string(setting_id_by_lookup_key(device_id, tag))
end

function storage_utils.find_nearest_restriction_event_time()
    return storage.get_table("2gig_pin_code_restriction_nearest_event_time")
end

function storage_utils.find_house_mode_to_change()
    return storage.get_string("2gig_house_mode_to_change")
end

return storage_utils
