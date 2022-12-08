local old_setting, new_setting = ...

local function merge_values()
    if old_setting.value_type == new_setting.value_type then
        new_setting.value = old_setting.value
    end
    
    -- TODO Add converters, when required
end


-- main


new_setting.id = old_setting.id
merge_values()
