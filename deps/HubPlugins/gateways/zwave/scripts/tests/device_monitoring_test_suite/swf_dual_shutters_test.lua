local tests = require( "tests" )

local zwave = require( "zwave" )
local core = require( "core" )
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local set_item_value = hubplugins.weak_loadfile("HUB:zwave/scripts/set_item_value")

zwave.request_node_values = tests.create_mock()

local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/swf_shutter_fixture")()

local function test_impl( product_info )
    local device_id_9000 = fixture:generate_device(9000, product_info)
    local device_id_9001 = fixture:generate_device(9001, product_info)
    local device_id_9002 = fixture:generate_device(9002, product_info)

    local not_swf_shade_product_info = {
        manufacturer_id = 1,
        product_type = 2,
        product_id = 3
    }

    local device_id_not_swf_shade = fixture:generate_device(9003, not_swf_shade_product_info)

    local second_shade_item_9000 = fixture:find_shutter_second_part_item_id(9000)
    local second_shade_item_9001 = fixture:find_shutter_second_part_item_id(9001)
    local second_shade_item_9002 = fixture:find_shutter_second_part_item_id(9002)

    -- test initial values
    local initial_value = {
        deviceId = "",
        partFunction = "unknown"
    }

    fixture:check_shutter_second_part_value(9000, initial_value)
    fixture:check_shutter_second_part_value(9001, initial_value)
    fixture:check_shutter_second_part_value(9002, initial_value)

    -- test correct case, opposite is undefined
    set_item_value({item_id = second_shade_item_9000, value = { deviceId = device_id_9001, partFunction = "front" } })
    fixture:check_shutter_second_part_value(9000, { deviceId = device_id_9001, partFunction = "front" })

    -- test correct case, opposite already seted
    set_item_value({item_id = second_shade_item_9001, value = { deviceId = device_id_9000, partFunction = "back" } })
    fixture:check_shutter_second_part_value(9001, { deviceId = device_id_9000, partFunction = "back" })

    -- test incorrect case, operated device in another pair
    set_item_value({item_id = second_shade_item_9000, value = { deviceId = device_id_9002, partFunction = "front" } })
    fixture:check_shutter_second_part_value(9000, { deviceId = device_id_9001, partFunction = "front" })

    -- test incorrect case, specified device in another pair
    set_item_value({item_id = second_shade_item_9002, value = { deviceId = device_id_9000, partFunction = "front" } })
    fixture:check_shutter_second_part_value(9002, initial_value)

    -- test clear association
    set_item_value({item_id = second_shade_item_9000, value = initial_value })
    set_item_value({item_id = second_shade_item_9001, value = initial_value })
    fixture:check_shutter_second_part_value(9000, initial_value)
    fixture:check_shutter_second_part_value(9001, initial_value)

    -- test incorrect case, specified device is not from swf
    set_item_value({item_id = second_shade_item_9000, value = { deviceId = device_id_not_swf_shade, partFunction = "front" } })
    fixture:check_shutter_second_part_value(9000, initial_value)

    -- remove node
    fixture:remove_node(9000, product_info)
    fixture:remove_node(9001, product_info)
    fixture:remove_node(9002, product_info)
    fixture:remove_node(9003, not_swf_shade_product_info)
end

tests.run_with_params( test_impl, fixture:get_dual_shutters_manufacturer_specifics() )
