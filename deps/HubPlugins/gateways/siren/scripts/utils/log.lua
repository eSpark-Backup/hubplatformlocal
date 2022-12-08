local PLUGIN = "siren"

local function ts()
    return os.date("%F %T.000000 " .. PLUGIN .. ": ")
end

-- enum PriorityLevel { FATAL = 0, ERROR = 300, WARN = 400, INFO = 600, DEBUG = 700, TRACE = 799 };
local log_level_enum = {
    error = 300,
    warning = 400,
    info = 600,
    debug = 700,
    trace = 799
}

local inner_log_level = log_level_enum.info

local logging = require("logging")
if logging ~= nil then
    inner_log_level = log_level_enum[logging.get_level()] or log_level_enum.info
end

local function set_log_level(level)
    inner_log_level = level
end
local function log_level()
    return inner_log_level
end

local function debug(...)
    if inner_log_level >= log_level_enum.debug then
        print(ts() .. " DEBUG: ", ...)
    end
end
local function trace(...)
    if inner_log_level >= log_level_enum.trace then
        print(ts() .. " TRACE: ", ...)
    end
end

local function error(...)
    if inner_log_level >= log_level_enum.error then
        print(ts() .. " ERROR: ", ...)
    end
end

local function info(...)
    if inner_log_level >= log_level_enum.info then
        print(ts() .. " INFO : ", ...)
    end
end

local function log(...)
    info(...)
end

local function dump(arg, prefix)
    prefix = prefix or "empty"
    if type(arg) == "table" then
        for k, v in pairs(arg) do
            dump(v, prefix .. "." .. k)
        end
    else
        print(prefix .. ": " .. tostring(arg))
    end
end

local function trace_dump(arg, prefix)
    if inner_log_level >= log_level_enum.trace then
        dump(arg, prefix)
    end
end

local logger = {
    log_level = log_level,
    set_log_level = set_log_level,
    log_level_enum = log_level_enum,
    log = log,
    info = info,
    error = error,
    debug = debug,
    trace = trace,
    trace_dump = trace_dump,
    PLUGIN = PLUGIN,
    dump = dump
}

return logger