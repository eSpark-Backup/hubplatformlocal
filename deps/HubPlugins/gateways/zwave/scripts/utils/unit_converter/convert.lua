local self, quantity, from, to = ...

if from == to or quantity == nil then
    return quantity
end

local conversion_data, base = self:call("_" .. from)

local function convert_impl(convertor)
    return type(convertor) == "function" and convertor(quantity) or
       type(convertor) == "number" and quantity * convertor or
       nil
end

if base == nil then
    return convert_impl(conversion_data[to])
else
    return self:call("convert", convert_impl(conversion_data), base, to) 
end

local convertor = conversion_data[to]
return type(convertor) == "function" and convertor(quantity) or
       type(convertor) == "number" and quantity * convertor or
       nil