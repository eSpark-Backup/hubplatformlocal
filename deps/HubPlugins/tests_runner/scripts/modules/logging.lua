local logging = {}

local level_mapping = {
    trace = 1,
    debug = 2,
    info = 3,
    warning = 4,
    error = 5
}

function logging.get_level()
    return _TESTS_LOG_LEVEL;
end

function logging.log(severity, message)
    if level_mapping[severity] >= level_mapping[_TESTS_LOG_LEVEL] then
        _test_print(severity .. ": " .. tostring(message))
    end
end


function logging.trace(message)
    logging.log("trace", message)
end


function logging.debug(message)
    logging.log("debug", message)
end

function logging.info(message)
    logging.log("info", message)
end


function logging.warning(message)
    logging.log("warning", message)
end


function logging.error(message)
    logging.log("error", message)
end

return logging
