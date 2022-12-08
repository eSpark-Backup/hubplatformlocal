local callback_function = ...
callback_function = callback_function or (function() return function () end end)

return setmetatable({ }, { __index = callback_function })
