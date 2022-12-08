local self, items  = ...

local item_names = require("HUB:zwave/scripts/resources/item_names")
local cc_ids = require("HUB:zwave/scripts/resources/class_ids")

local function add_item_to_merged_map(merged_items_map, result_map, item)
    assert(item.name)
    
    -- do not merge "button_state" items
    if item.name == item_names.button_state then 
        table.insert(result_map, { main_item = item, descriptors = { item.descriptor } })
        return
    end

    local merged_item_index = merged_items_map.names_map[item.name]
    if merged_item_index then
        local merged_item_info = merged_items_map.array[merged_item_index]
        
        if merged_item_info.main_item.descriptor.class_id == cc_ids.basic then
            local old_main_item = merged_item_info.main_item
            merged_item_info.main_item = item
            self:call("merge_items", merged_item_info.main_item, old_main_item)
            
            local new_descriptors = { item.descriptor }
            for _, descriptor in ipairs(merged_item_info.descriptors) do
                table.insert(new_descriptors, descriptor)
            end
            
            merged_item_info.descriptors = new_descriptors
        else
            self:call("merge_items", merged_item_info.main_item, item)
            table.insert(merged_item_info.descriptors, item.descriptor)
        end
    else
        table.insert(merged_items_map.array, { main_item = item, descriptors = { item.descriptor } })
        merged_items_map.names_map[item.name] = #merged_items_map.array
    end
end

local function add_security_treat(merged_items_map, result_map, item_info)
    if self:call("is_security_item", item_info.main_item) then
        local security_item = self:call("create_security_threat_item", item_info.main_item)
        add_item_to_merged_map(merged_items_map, result_map, security_item)
    end
end


local merged_items_map = { names_map = {}, array = {} }
local result_map = {}

-- add original items to map
for _, item in ipairs(items) do
     add_item_to_merged_map(merged_items_map, result_map, item)
end

-- move merged items to result
for _, item_info in pairs(merged_items_map.array) do
    table.insert(result_map, item_info)
end

-- clear merged items map
merged_items_map.names_map = {}
merged_items_map.array = {}
hubplugins.try_collectgarbage()

-- generate security_threat items based on result items
for _, item_info in pairs(result_map) do
    add_security_treat(merged_items_map, result_map, item_info)
end

-- move security_threat items to result
for _, item_info in pairs(merged_items_map.array) do
    table.insert(result_map, item_info)
end

merged_items_map = nil
hubplugins.try_collectgarbage()

return result_map
