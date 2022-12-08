local device_id = ...

local core = assert(require "core", "Fail to load core functionality")
local zigbee = assert(require("zigbee"), "Fail to load the 'zigbee' functionality")
local storage_helper = hubplugins.object("storage_helper", "HUB:zigbee/scripts/utils")
local attr_uniquefier = hubplugins.weak_loadfile("HUB:zigbee/scripts/utils/attribute_uniquefier")()
local related_attributes_getter = hubplugins.weak_loadfile("HUB:zigbee/scripts/helpers/get_related_attributes")

local function is_mfg_specific_item(descriptor)
    local mfg_specific_tag_helper = hubplugins.weak_loadfile("HUB:zigbee/scripts/helpers/mfg_specific_tag_helper")()
    return descriptor.tag ~= mfg_specific_tag_helper.remove_mfg_specific_tag(descriptor.tag)
end

local function request_mfg_specific_attr_value(descriptor, attr_name)
    local cluster_id = descriptor.cluster_name
    local attribute_id = attr_name

    if attr_uniquefier.unique_by_ids(
        descriptor.node_id,
        descriptor.endpoint_id,
        cluster_id,
        attribute_id ) then
        zigbee.request_mfg_specific_attribute_value(
            descriptor.node_id,
            descriptor.endpoint_id,
            cluster_id,
            attribute_id )
    end
end

local function request_attr_value(descriptor, attr_name)
    if attr_uniquefier.unique(
        descriptor.node_id,
        descriptor.endpoint_id,
        descriptor.cluster_name,
        attr_name ) then
        zigbee.request_attribute_value(
            descriptor.node_id,
            descriptor.endpoint_id,
            descriptor.cluster_name,
            attr_name )
    end
end

local function custom_request_action(item, descriptor)
    local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")

    if descriptor.item_name == item_names.user_codes and descriptor.cluster_name == "door_lock" then
        zigbee.request_user_codes(descriptor.node_id, descriptor.endpoint_id, 1, item.elements_max_number)
    end

    if descriptor.cluster_name == "appliance_events_and_alerts" then
        zigbee.send_command(descriptor.node_id, descriptor.endpoint_id, descriptor.cluster_name, "GetAlertsResponse")
    end

    if item.name ==  item_names.zones_status then
        local house_modes = assert(require "house_modes", "Fail to load core functionality")
        local house_mode = house_modes.get_current()

        local values_converters = hubplugins.weak_loadfile("HUB:zigbee/scripts/resources/values_converters/ias_ace")()
        local new_zone_status = values_converters.house_mode_to_zone_status[house_mode]
        if new_zone_status then
            core.update_item_value(item.id, new_zone_status)
        end
    end
end


-- main


local callback = function( devive )
    local items = core.get_items_by_device_id(devive.id)
    if items then
        for _, item in ipairs(items) do
            local descriptors = storage_helper:call("find_item_descriptors", item.id)
            for _, descriptor in ipairs( descriptors ) do
                local attributes = related_attributes_getter(descriptor)
                if attributes then
                    for _,attr_name in ipairs(attributes) do
                        if is_mfg_specific_item(descriptor) then
                            request_mfg_specific_attr_value(descriptor, attr_name)
                        else
                            request_attr_value(descriptor, attr_name)
                        end
                    end
                end
                custom_request_action(item, descriptor)
            end
        end
    end

    local setting_ids = core.get_setting_ids_by_device_id(devive.id)
    if setting_ids then
        for _, setting_id in ipairs(setting_ids) do
            local descriptor = storage_helper:call("find_setting_descriptor", setting_id)
            if descriptor then
                request_attr_value(descriptor, descriptor.attribute_name)
            end
        end
    end
end

print("Request all device values for device_id: " .. device_id)
hubplugins.weak_loadfile("HUB:zigbee/scripts/helpers/for_each_device")(device_id, callback, core)
