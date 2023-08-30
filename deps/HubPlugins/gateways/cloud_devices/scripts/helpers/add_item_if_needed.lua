local core_device_id, capability = ...

local core = assert(require "core", "Fail to load core functionality")

if capability == "text_command" then
    print( "adding text_command item" )
    core.add_item({
        name = "text_command",
        value_type = "string",
        value = "",
        has_getter = false,
        has_setter = true,
        device_id = core_device_id
    })
end
