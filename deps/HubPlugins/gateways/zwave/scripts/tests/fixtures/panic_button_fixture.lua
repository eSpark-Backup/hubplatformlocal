local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/base_fixture")()

local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )

function fixture.add_device_specific_data(self, data_to_specify)
    local old_loadfile = hubplugins.weak_loadfile
    hubplugins.weak_loadfile = function(path)
        if path == "HUB:zwave/scripts/model/devices/devices_info/generated/999_0_0" then
            return function()
                local info = old_loadfile("HUB:zwave/scripts/model/devices/devices_info/base_info")()
                for key, value in pairs(data_to_specify) do
                    info[key] = value
                end
                return info
            end
        else
            return old_loadfile(path)
        end
    end
end

-- notifications
function fixture.add_device_with_intrusion_alarm(self, device_specific_data)
    self:add_device_specific_data(device_specific_data)
    
    local node_desc = self:get_node_with_intrusion_alarm_description()
    self:add_node(node_desc)
end

function fixture.remove_device_with_intrusion_alarm(self)
    local node_desc = self:get_node_with_intrusion_alarm_description()
    self:remove_node(node_desc)
end

-- central scene
function fixture.add_device_with_central_scene(self, device_specific_data)
    self:add_device_specific_data(device_specific_data)
    
    local node_desc = self:get_node_with_central_scene_description()
    self:add_node(node_desc)
end

function fixture.remove_device_with_central_scene(self)
    local node_desc = self:get_node_with_central_scene_description()
    self:remove_node(node_desc)
end

-- nodes descriptions
function fixture.get_node_with_central_scene_description(self)
    local cc_central_scene = {
        id = class_ids.central_scene,
        buttons = {
            {button_id = 1, button_attributes = {"press_1_time", "released", "held_down"}},
            {button_id = 2, button_attributes = {"press_1_time", "released", "held_down"}},
            {button_id = 3, button_attributes = {"press_1_time", "released", "held_down"}},
        }
    }
    
    local channel_0 = {
        id = 0,
        classes = { cc_central_scene }
    }
    
    return {
        id = "9000",
        manufacturer_id = 999,
        product_type = 0,
        product_id = 0,
        is_battery_powered = true,
    
        specific_type = "GENERIC_TYPE_WALL_CONTROLLER",
        generic_type = "GENERIC_TYPE_WALL_CONTROLLER",
    
        security = { "none" },
        security_class_started = { "none" },
    
        channels = { channel_0 }
    }
end

function fixture.get_node_with_intrusion_alarm_description(self)
    local cc_central_scene = {
        id = class_ids.central_scene,
        buttons = {
            {button_id = 1, button_attributes = {"press_1_time", "released", "held_down"}},
            {button_id = 2, button_attributes = {"press_1_time", "released", "held_down"}},
            {button_id = 3, button_attributes = {"press_1_time", "released", "held_down"}},
        }
    }
    
    local cc_notification = {
    id = class_ids.notification,
    notifications = {
        {
            type = "home_security",
            event_types = { 
                "intrusion",
                "impact_detected"
            }
        },
    }
}
    
    local channel_0 = {
        id = 0,
        classes = { cc_central_scene, cc_notification }
    }
    
    return {
        id = "9000",
        manufacturer_id = 999,
        product_type = 0,
        product_id = 0,
        is_battery_powered = true,
    
        specific_type = "GENERIC_TYPE_WALL_CONTROLLER",
        generic_type = "GENERIC_TYPE_WALL_CONTROLLER",
    
        security = { "none" },
        security_class_started = { "none" },
    
        channels = { channel_0 }
    }
end

return fixture
