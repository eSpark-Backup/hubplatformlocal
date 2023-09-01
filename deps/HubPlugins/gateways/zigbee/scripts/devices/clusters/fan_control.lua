local cls_fan_control = hubplugins.weak_loadfile("HUB:zigbee/scripts/devices/clusters/base")()

function cls_fan_control.process_cluster(self, inclusion_context)
    local sequence_attr = self:find_attribute("FanModeSequence", inclusion_context.cluster)
    assert(sequence_attr, "Can not find FanModeSequence attribute")
    if type(sequence_attr.formatted_value) ~= "table" or #sequence_attr.formatted_value == 0 then
        return
    end

    local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")

    local item = self:create_item(inclusion_context, "FanMode", item_names.thermostat_fan_mode)

    local helper = hubplugins.weak_loadfile("HUB:zigbee/scripts/helpers/fan_control_helper")()
    for _, attr_value in ipairs(sequence_attr.formatted_value) do
        assert(helper.attr_value_to_enum[attr_value], "Unsupported fan mode attr value: " .. attr_value)
        table.insert(item.enum, helper.attr_value_to_enum[attr_value])
    end

    item.value = helper.attr_value_to_enum[sequence_attr.formatted_value[1]]

    self:add_items_and_settings(inclusion_context, {item})
end

return cls_fan_control
