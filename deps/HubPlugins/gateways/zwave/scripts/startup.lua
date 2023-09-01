local zwave = require "zwave"
if not zwave then
    print( "Fail to load the zwave functionality" )
    return
end

require("HUB:zwave/scripts/helpers/scripts_preload")

zwave.subscribe( "HUB:zwave/scripts/events/events_handling" )
zwave.subscribe( "HUB:zwave/scripts/model/gateway/events_handling/events_handling" )
zwave.set_default_device_configurations_file( "HUB:zwave/configs/device_setting.json" )

-- initialize gateway settings
local gateway_initialization_handler = hubplugins.object( "initialization", "HUB:zwave/scripts/model/gateway" )
gateway_initialization_handler:call("ensure_gateway_settings")
-- setup polling intervals
local gateway_monitoring_handler = hubplugins.object( "monitoring", "HUB:zwave/scripts/model/gateway" )
gateway_monitoring_handler:call("init_all_settings_values")

hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/update_gateway_roles")()

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
storage:call("set_storage_version", hubplugins.weak_require("HUB:zwave/scripts/resources/version"))
storage:call("remove_all_device_checking_ids")

hubplugins.weak_require("HUB:zwave/scripts/helpers/request_all_node_values")

hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/for_each_zwave_node_info")(function(node_info)
    storage:call("delete_table", storage:call("get_monitoring_handler_key", node_info.id)) -- remove cached monitoring name (for case scripts have been updated)

    hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/start_daemon")(node_info.id, node_info.manufacturer_id, node_info.product_type, node_info.product_id)

    -- rediscover device if new feateres has been added in znet for this node
    -- TODO: maybe more sophisticated check
    if node_info.is_rediscover_required then
        zwave.start_rediscover(node_info.id, "HUB:zwave/configs/device_setting.json")
    end
end)

-- subscribe for core events to be able to monitor devices' changing for proper SmartStart devices handling
local smart_start = hubplugins.object( "smart_start", "HUB:zwave/scripts/utils" )
smart_start:call( "subscribe_for_core_events" )

storage:call("set_check_extensions_flag", true)

-- Process virtual pin codes restrictions
if hubplugins.virtual_pin_codes_restrictions_enabled then
    hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/virtual_pin_codes_restrictions/process_restrictions")()
end

-- init working with thermostat energy saving mode
local energy_saving_mode_handler = hubplugins.object("energy_saving_mode", "HUB:zwave/scripts/helpers")
energy_saving_mode_handler:call("init_startup_timer")
