local temperature_measurement = hubplugins.weak_loadfile("HUB:zigbee/scripts/devices/clusters/base")()

function temperature_measurement.process_cluster(self, inclusion_context)
    local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")

    for _, attribute in ipairs(inclusion_context.cluster.attributes) do
        if attribute.name == "MeasuredValue" then
            local item = self:create_item(inclusion_context, "MeasuredValue", item_names.temp)
            local setting = self:create_setting(inclusion_context, "MeasuredValue", "temperature_calibration")
            self:add_items_and_settings_to_child_device(inclusion_context, item.name, { item }, { setting })
        end
    end
end

return temperature_measurement
