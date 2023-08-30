local function impl(self, ...)
    local suffixes = { ... }

    return "c7db6e11-94b3-4236-b02c-8217f8871697/daemon/" .. table.concat(suffixes, "/")
end

return impl(...)
