local manufacturer_id, product_type, product_id, dev_handler_id = ...

if not dev_handler_id then
    dev_handler_id = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/create_device_type_id")(
        manufacturer_id,
        product_type,
        product_id)
end

local exclusion_class_location = "HUB:zwave/scripts/model/devices/exclusion"
return  hubplugins.object(dev_handler_id, exclusion_class_location)
    or  hubplugins.object("default", exclusion_class_location)
