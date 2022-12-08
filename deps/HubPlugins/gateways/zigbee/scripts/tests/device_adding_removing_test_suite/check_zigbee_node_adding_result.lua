local expected_devices, check_context = ...

if not check_context then
    check_context = {
        descriptors = {},
        item_ids = {},
        device_ids = {},
        setting_ids = {}
    }
end

local tests = require( "tests" )

local core = require( "core" )

local storage = hubplugins.object( "storage_helper", "HUB:zigbee/scripts/utils" )

local devices_count = 0
local items_count = 0
local settings_count = 0
local descriptors_count = 0
local main_and_merged_items_count = 0


local function compare_device_model_impl( root, expected )
    local device_id

    devices_count = devices_count + 1

    local function on_device_id_found_by_child( actual_dev_id )
        assert( actual_dev_id )
        if device_id then
            tests.assert_eq( actual_dev_id, device_id, "check dev id" )
        else
            device_id = actual_dev_id
        end
    end

    if expected.settings then
        for _, setting_info in ipairs( expected.settings ) do
            local setting_id = storage:call(
                "find_setting_id",
                {
                    setting_name = setting_info.name,
                    node_id = root.node_id,
                    endpoint_id = setting_info.endpoint_id,
                    cluster_name = setting_info.cluster_name,
                    attribute_name = setting_info.attribute_name
                }
            )

            settings_count = settings_count + 1

            tests.assert_ne( setting_id, nil, "Setting id must be present" )
            local setting = tests.assert_ne( core.get_setting( setting_id ), nil, "Setting must be present in core" )
            on_device_id_found_by_child(setting.device_id)

            tests.assert_ne(setting.value_type, nil, "setting value type must be present!")
            tests.assert_ne(setting.value, nil, "setting value must be present!")

            tests.assert_eq(setting.value_type, setting_info.value_type, "Check setting value type")

            tests.assert_ne(setting.description, nil, "check description")
            tests.assert_eq(type(setting.description.text), "string", "check description text type")

            tests.assert_ne(setting.label, nil, "check label")
            tests.assert_eq(type(setting.label.text), "string", "check label text type")

            tests.assert_eq( setting.enum, setting_info.enum, "check settings enum" )

            tests.assert_eq(setting.value_default, setting_info.value_default, "Check setting default value")
            tests.assert_eq(setting.value_min, setting_info.value_min, "Check setting min value")
            tests.assert_eq(setting.value_max, setting_info.value_max, "Check setting max value")

            tests.assert_eq(setting.info, setting_info.info, "Check setting info")

            table.insert(check_context.setting_ids, setting_id)
        end
    end

    if expected.items then
        for _, item_info in ipairs( expected.items ) do
            local main_item_name = item_info.main_item_name or item_info.name
            local item_id =
                storage:call(
                    "find_item_id",
                    {
                        item_name = main_item_name,
                        node_id = root.node_id,
                        endpoint_id = item_info.endpoint_id,
                        cluster_name = item_info.cluster_name,
                        attribute_name = item_info.attribute_name,
                        tag = item_info.tag
                    }
                )

            tests.assert_ne( item_id, nil, "Item " .. main_item_name .. "|" .. tostring( item_info.tag ) ..  " must be present" )
            local item = tests.assert_ne( core.get_item( item_id ), nil, "Item must be present in core" )

            tests.assert_eq( item_info.name, item.name, "check item name" )

            tests.assert_ne( item.value, nil, "check default value is passed to item" )
            if item_info.value then
                tests.assert_eq( item.value, item_info.value, "check item value" )
            end

            if item_info.value_type then
                tests.assert_eq( item_info.value_type, item.value_type, "check item value type" )

                local actual_enum_set = tests.assert_unique( item_info.enum, "item enum" )
                local expected_enum_set = tests.assert_unique( item.enum, "expected enum" )

                tests.assert_eq( actual_enum_set, expected_enum_set, "check item enum" )
            end

            if item_info.elements_max_number then
                tests.assert_eq( item.elements_max_number, item_info.elements_max_number, "check item element max number" )
            end

            if item_info.user_code_restriction then
                tests.assert_eq( item.user_code_restriction, item_info.user_code_restriction, "check item user code restriction" )
            end

            local descriptors = storage:call( "find_item_descriptors", item_id )
            tests.assert_ne( descriptors, nil, "check descriptors" )

            local index
            for i, descriptor in ipairs( descriptors ) do
                if descriptor.item_name == main_item_name and
                   descriptor.node_id == root.node_id and
                   descriptor.endpoint_id == item_info.endpoint_id and
                   descriptor.cluster_name == item_info.cluster_name and
                   descriptor.attribute_name == item_info.attribute_name and
                   descriptor.tag == item_info.tag then
                    tests.assert_eq( index, nil, "only one descriptor" )
                    index = i
                end

                if not item_info.is_merged then
                    table.insert( check_context.descriptors, descriptor )
                end
            end

            local main_descriptor = storage:call( "find_item_descriptor", item_id )

            tests.assert_ne( index, nil, "descriptor must be present in storage" )
            tests.assert_eq( item_info.is_merged == true, index > 1, "merged item can't be first" )

            on_device_id_found_by_child( item.device_id )

            if not item_info.is_merged then
                items_count = items_count + 1
                table.insert( check_context.item_ids, item_id )

                descriptors_count = descriptors_count + #descriptors
            end

            main_and_merged_items_count = main_and_merged_items_count + 1
        end
    end

    if expected.subdevices then
        for _, device_info in ipairs( expected.subdevices ) do
            local device = compare_device_model_impl( root, device_info )
            tests.assert_ne( device.parent_device_id, nil, "parent device id must be present for children" )
            on_device_id_found_by_child( device.parent_device_id )
        end
    end

    local function check_device_field( actual_dev, expected_dev, field_name )
        if expected_dev[field_name] ~= nil then
            tests.assert_eq( actual_dev[field_name], expected_dev[field_name], "check device field '" .. field_name .. "'" )
        end
    end

    tests.assert_ne( device_id, nil, "device id must be found" )
    local device = tests.assert_ne( core.get_device( device_id ), nil, "device must be present" )

    tests.assert_eq( device.id, device_id, "check device id" )
    check_device_field( device, expected, "category" )
    check_device_field( device, expected, "subcategory" )
    check_device_field( device, expected, "name" )
    check_device_field( device, root, "battery_powered" )
    check_device_field( device, expected, "type" )
    check_device_field( device, expected, "status" )

    table.insert( check_context.device_ids, device_id )

    return device
end

-- main
for _, expected in ipairs( expected_devices ) do
    local actual_dev_id = storage:call( "find_device_id", expected.node_id )
    tests.assert_ne( actual_dev_id, nil, "node_id id " .. expected.node_id .. " must be present in map" )
    tests.assert_eq( storage:call( "find_node_id", actual_dev_id ), expected.node_id, "check device-node bimap" )

    local device = core.get_device( actual_dev_id )
    tests.assert_ne( device, nil, "device must be present in core" )

    compare_device_model_impl( expected, expected )
end

local core_items_count = #core.get_items()
local core_devices_count = #core.get_devices()
local core_settings_count = #core.get_settings()
tests.assert_eq( core_items_count - ( check_context.items_count or 0 ), items_count, "check extra items" )
tests.assert_eq( core_devices_count - ( check_context.devices_count or 0 ), devices_count, "check extra devices" )
tests.assert_eq( core_settings_count - ( check_context.settings_count or 0 ), settings_count, "check extra settings" )
tests.assert_eq( main_and_merged_items_count, descriptors_count, "descriptors count must be the same as all items count" )

check_context.devices_count = core_devices_count
check_context.items_count = core_items_count
