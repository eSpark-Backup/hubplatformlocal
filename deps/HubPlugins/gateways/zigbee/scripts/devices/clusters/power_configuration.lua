local power_configuration = hubplugins.weak_loadfile("HUB:zigbee/scripts/devices/clusters/base")()

function power_configuration.process_cluster(self, inclusion_context)
    local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")

    if inclusion_context.endpoint_id ~= 1 then return end

    local attr_name

    if self:find_attribute("BatteryPercentageRemaining", inclusion_context.cluster) then
        attr_name = "BatteryPercentageRemaining"
    elseif self:find_attribute("BatteryVoltage", inclusion_context.cluster) then
        attr_name = "BatteryVoltage"
    end

    if attr_name then
        local item = self:create_item(inclusion_context, attr_name, item_names.battery)
        self:add_items_and_settings(inclusion_context, { item })
    end
end

return power_configuration
