local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/base_fixture")()

local zwave = require( "zwave" )
local core = require( "core" )
local tests = require( "tests" )
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local item_names = require("HUB:zwave/scripts/resources/item_names")
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

local channel_0 = { 
    id = 0,
    classes = { 
        { id = class_ids.thermostat_mode,
            modes = { 0,1,2,7,13 }
        },
        { id = class_ids.thermostat_setpoint,
            setpoints = {
                {type = 1},
                {type = 2},
                {type = 9},
                {type = 15}
            }
        }
    }
}

fixture.node = {
    id = "9000",
    manufacturer_id = 1,
    product_type = 2,
    product_id = 3,
    is_battery_powered = true,

    specific_type = "",
    generic_type = "",

    security = { "none" },
    security_class_started = { "none" },

    channels = { channel_0 }
}

fixture.setpoints_spread = 0

function fixture.init(self)
    tests.override_function(_G, "loadfile", function(orig_loadfile, path)
        if path == "HUB:zwave/scripts/model/devices/devices_info/generated/1_2_3" then
            return function()
                local info = orig_loadfile("HUB:zwave/scripts/model/devices/devices_info/base_info")()
                info.thermostat_setpoints_spread = fixture.setpoints_spread
                return info
            end
        else
            return orig_loadfile(path)
        end
    end)

    zwave.request_node_values = tests.create_mock()
    zwave.set_setpoint_value = tests.create_mock()

    self:add_node(fixture.node)
    
    self.setpoint_heating_item_id = self:get_item_id(class_ids.thermostat_setpoint, item_names.thermostat_setpoint_heating)
    self.setpoint_cooling_item_id = self:get_item_id(class_ids.thermostat_setpoint, item_names.thermostat_setpoint_cooling)
    self.energy_saving_mode_item_id = self:get_item_id(class_ids.thermostat_setpoint, item_names.thermostat_energy_saving_mode)
    self.mode_item_id = self:get_item_id(class_ids.thermostat_mode, item_names.thermostat_mode)
    
    core.update_item_value(fixture.mode_item_id, "auto")
    self:show_item(self.setpoint_heating_item_id, true)
    self:show_item(self.setpoint_cooling_item_id, true)
end

function fixture.get_item_id(self, class_id, item_name)
    local item_id = storage:call("find_item_id", item_name, "9000", class_id, 0)
    tests.assert_ne(item_id, nil, item_name .. " not found")

    return item_id
end

function fixture.show_item(self, item_id, show)
    local item = core.get_item(item_id)
    item.show = show
    core.modify_item(item_id, item)
end

function fixture.set_setpoints_spread(self, value)
    self.setpoints_spread = value
end

return fixture
