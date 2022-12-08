local self = ...

return {
    celsius = function(value)
        return (value - 32) * 5 / 9
    end
}
