local setting = ...

if not setting.configuration then return false end
if not setting.configuration.number then return false end
if not setting.configuration.value then return false end

return true
