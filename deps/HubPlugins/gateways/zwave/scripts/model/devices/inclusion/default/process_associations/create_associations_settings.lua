local self, device_data, zwave_node = ...

local assoc_groups_setting = self:call("process_associations/prepare_association_groups_setting", zwave_node)
if assoc_groups_setting then
    table.insert(device_data.settings, assoc_groups_setting)
    table.insert(device_data.settings , self:call("process_associations/prepare_associations_setting", zwave_node))
end
