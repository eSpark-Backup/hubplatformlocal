local dev_data = ...

local core = assert(require "core", "Fail to load core functionality")
local zigbee = assert(require("zigbee"), "Fail to load the 'zigbee' functionality")
local storage_helper = hubplugins.object("storage_helper", "HUB:zigbee/scripts/utils")
local attr_uniquefier = hubplugins.weak_loadfile("HUB:zigbee/scripts/utils/attribute_uniquefier")()
local related_attributes_getter = hubplugins.weak_loadfile("HUB:zigbee/scripts/helpers/get_related_attributes")

local function is_mfg_specific_item(descriptor)
    local mfg_specific_tag_helper = hubplugins.weak_loadfile("HUB:zigbee/scripts/helpers/mfg_specific_tag_helper")()

    return descriptor.tag ~= mfg_specific_tag_helper.remove_mfg_specific_tag(descriptor.tag)
end

local function configure_mfg_specific_attribute(descriptor, attr_name)
    local cluster_id = descriptor.cluster_name
    local attribute_id = attr_name

    if not attr_uniquefier.unique_by_ids(
        descriptor.node_id,
        descriptor.endpoint_id,
        cluster_id,
        attribute_id ) then
        return
    end

    local attribue_conf = nil
    if dev_data.info.mfg_specific_attr_configurations and dev_data.info.mfg_specific_attr_configurations[tostring(math.floor(cluster_id))] then
        attribue_conf = dev_data.info.mfg_specific_attr_configurations[tostring(math.floor(cluster_id))][tostring(math.floor(attribute_id))]
    end

    if attribue_conf then
        zigbee.configure_mfg_specific_attribute_report(
            descriptor.node_id,
            descriptor.endpoint_id,
            cluster_id,
            attribute_id,
            attribue_conf.min_interval,
            attribue_conf.max_interval,
            attribue_conf.reportable_change)
    else
        zigbee.configure_mfg_specific_attribute_report(
            descriptor.node_id,
            descriptor.endpoint_id,
            cluster_id,
            attribute_id)
    end
end

local function configure_attribute(descriptor, attr_name)
    if not attr_uniquefier.unique(
        descriptor.node_id,
        descriptor.endpoint_id,
        descriptor.cluster_name,
        attr_name ) then
        return
    end

    local attribue_conf = nil
    if dev_data.info.attr_configurations and dev_data.info.attr_configurations[descriptor.cluster_name] then
        attribue_conf = dev_data.info.attr_configurations[descriptor.cluster_name][attr_name]
    end

    if attribue_conf then
        zigbee.configure_attribute_report(
            descriptor.node_id,
            descriptor.endpoint_id,
            descriptor.cluster_name,
            attr_name,
            attribue_conf.min_interval,
            attribue_conf.max_interval,
            attribue_conf.reportable_change)
    else
        zigbee.configure_attribute_report(
            descriptor.node_id,
            descriptor.endpoint_id,
            descriptor.cluster_name,
            attr_name)
    end
end


-- main


for _, item in ipairs(dev_data.items) do
    local attributes = related_attributes_getter(item.descriptor)
    if attributes then
        for _,attr_name in ipairs(attributes) do
            if is_mfg_specific_item(item.descriptor) then
                configure_mfg_specific_attribute(item.descriptor, attr_name)
            else
                configure_attribute(item.descriptor, attr_name)
            end
        end
    end
end

for _, setting in ipairs(dev_data.settings) do
    configure_attribute(setting.descriptor, setting.descriptor.attribute_name)
end
