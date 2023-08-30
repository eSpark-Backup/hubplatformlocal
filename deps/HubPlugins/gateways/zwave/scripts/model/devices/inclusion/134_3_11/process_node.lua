local self, zwave_node = ...

if #zwave_node.channels == 5 then
    local meter_cc_id = require("HUB:zwave/scripts/resources/class_ids").meter

    local meter_cc
    for _, cc in ipairs(zwave_node.channels[1].classes) do
        if cc.id == meter_cc_id then
            meter_cc = cc
            break
        end
    end

    if meter_cc then
        for i = 5, 6 do
            table.insert(zwave_node.channels, { id = i, classes = { meter_cc } })
        end
    else
        print("Meter cc not found in DSC11 ZWUS!!!")
    end
else
    print("Unexpected channels count " .. #zwave_node.channels .. " for DSC11 ZWUS!!!")
end

return self:_super_call("default", "process_node", zwave_node)
