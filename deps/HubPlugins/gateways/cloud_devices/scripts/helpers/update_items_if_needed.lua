local core_device_id, capabilities = ...

local core = assert(require "core", "Fail to load core functionality")

local core_items = core.get_items_by_device_id(core_device_id)
local core_items_set = {}
if core_items then
    for _, core_item in ipairs(core_items) do
        core_items_set[core_item.name] = true
    end
end

local capabilities_set = {}
if capabilities then
    local add_item_if_needed = loadfile("HUB:cloud_devices/scripts/helpers/add_item_if_needed")
    for _, capability in ipairs(capabilities) do
        capabilities_set[capability] = true

        if not core_items_set[capability] then
            add_item_if_needed(core_device_id, capability);
        end
    end
end

if core_items then
    for _, core_item in ipairs(core_items) do
        if not capabilities_set[core_item.name] then
            print("removing " .. core_item.name .. " item")
            core.remove_item(core_item.id)
        end
    end
end
