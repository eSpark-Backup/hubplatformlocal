local cls_color_control = hubplugins.weak_loadfile("HUB:zigbee/scripts/devices/clusters/base")()

function cls_color_control.process_cluster(self, inclusion_context)
    local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")
    local item = self:create_item(inclusion_context, "", item_names.rgb_color)

    local color_capabilities_attr = self:find_attribute("ColorCapabilities", inclusion_context.cluster)

    if  self:find_attribute("CurrentX", inclusion_context.cluster)
    and self:has_capability(color_capabilities_attr, "xy_attributes_supported") then
        assert(self:find_attribute("CurrentY", inclusion_context.cluster))
        item.value.red = 0
        item.value.green = 0
        item.value.blue = 0
    end

    if  self:find_attribute("ColorTemperatureMireds", inclusion_context.cluster)
    and self:has_capability(color_capabilities_attr, "color_temperature_supported") then
        assert(self:find_attribute("ColorMode", inclusion_context.cluster))
        item.value.cwhite = 0
        item.value.wwhite = 0
    end

    if next(item.value) then
        self:add_items_and_settings(inclusion_context, { item })
    end
end

function cls_color_control.has_capability(self, color_capabilities_attr, capability)
    if not color_capabilities_attr then return true end

    if not color_capabilities_attr.formatted_value then return false end

    for _, existing_cap in ipairs(color_capabilities_attr.formatted_value) do
        if existing_cap == capability then return true end
    end

    return false
end

return cls_color_control
