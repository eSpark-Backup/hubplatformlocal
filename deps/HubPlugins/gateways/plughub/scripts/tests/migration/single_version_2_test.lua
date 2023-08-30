local core = require("core")
local tests = require("tests")
local fixture = require("HUB:plughub/scripts/tests/migration/migration_fixture")

local function add_device()
    return core.add_device({
        gateway_id = core.get_gateway().id,
        name = "PowerPlug",
        device_type_id = "",
        type = "switch.outlet",
        category = "switch",
        subcategory = "in_wall",
        battery_powered = true,
        reachable = true
    })
end

local function add_item(device_id, item_name)
    return core.add_item({
        device_id = device_id,
        name = item_name,
        value_type = "float",
        value = 123.456,
        has_getter = true,
        has_setter = false
    })
end

local function check_item(item_id, value_type, value)
    local item = core.get_item(item_id)

    tests.assert_eq(item.value_type, value_type)
    tests.assert_eq(item.value, value)
end

local device_id = add_device()
local item_id_1 = add_item(device_id, "electric_meter_amper")
local item_id_2 = add_item(device_id, "electric_meter_kwh")
local item_id_3 = add_item(device_id, "electric_meter_volt")
local item_id_4 = add_item(device_id, "electric_meter_watt")
local item_id_5 = add_item(device_id, "another_item")

fixture.get_migration_routine(2).migrate_up()

check_item(item_id_1, "electric_current",        { value = 123.456, scale = "ampere" })
check_item(item_id_2, "amount_of_useful_energy", { value = 123.456, scale = "kilo_watt_hour" })
check_item(item_id_3, "electric_potential",      { value = 123.456, scale = "volt" })
check_item(item_id_4, "power",                   { value = 123.456, scale = "watt" })
check_item(item_id_5, "float", 123.456)

fixture.get_migration_routine(2).migrate_down()

check_item(item_id_1, "float", 123.456)
check_item(item_id_2, "float", 123.456)
check_item(item_id_3, "float", 123.456)
check_item(item_id_4, "float", 123.456)
check_item(item_id_5, "float", 123.456)
