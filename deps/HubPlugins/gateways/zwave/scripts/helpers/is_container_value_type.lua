local value_type = ...

function starts_with(str, substr)
   return string.sub(str, 1, string.len(substr)) == substr
end

return starts_with(value_type, "array.") or starts_with(value_type, "dictionary.")
