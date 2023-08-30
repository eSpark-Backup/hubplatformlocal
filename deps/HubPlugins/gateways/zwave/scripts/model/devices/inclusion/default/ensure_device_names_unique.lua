local self, inclusion_context = ...

local device_tree_model = inclusion_context.device_tree_model

local names_indexes = {  }
local first_devices_info = {  }

local root = device_tree_model:call("get_root_node")

device_tree_model:call("direct_stable_walk", function(dev_node)
    local dev_info = device_tree_model:call("get_data", dev_node).info
    local name = dev_info.name

    if names_indexes[ name ] then
        if first_devices_info[ name ] then
            first_devices_info[ name ].name = name .. " 1"
            first_devices_info[ name ] = nil
        end

        names_indexes[ name ] = names_indexes[ name ] + 1
        dev_info.name = name .. " " .. names_indexes[ name ]
    elseif dev_node == root then
        names_indexes[ name ] = 0
    else
        names_indexes[ name ] = 1
        first_devices_info[ name ] = dev_info
    end
end)
