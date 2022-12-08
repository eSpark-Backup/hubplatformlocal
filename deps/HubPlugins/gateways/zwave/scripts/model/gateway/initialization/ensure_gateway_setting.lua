local self, setting_name = ...

local core = require("core")

local setting_id = self:call("generate_gateway_setting_id", setting_name)

if not self:call("get_gateway_setting", setting_id) then
    self:call("add_gateway_setting", setting_name)
end
