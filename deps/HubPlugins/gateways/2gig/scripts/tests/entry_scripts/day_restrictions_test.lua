local test_utils = require("HUB:2gig/scripts/tests/test_utils")
local add_device = loadfile("HUB:2gig/scripts/add_device")
local item_factory = require("HUB:2gig/scripts/helpers/item_factory")
local item_names = require("HUB:2gig/scripts/enum/item_names")
local add_item_dict_value = loadfile("HUB:2gig/scripts/add_item_dictionary_value")
local set_item_dict_value = loadfile("HUB:2gig/scripts/set_item_dictionary_value")
local rem_item_dict_value = loadfile("HUB:2gig/scripts/remove_item_dictionary_value")
local clear_item_dict = loadfile("HUB:2gig/scripts/clear_item_dictionary")
local tests = require("tests")
local core = require("core")

core.get_local_time = function(...) return "2022-01-01T01:01:01" end

add_device({
    model = "2GIG-PAD1-345",
    serial = 123
})

local pin_codes_item_id = test_utils.get_item_id(123, item_names.user_codes)
local daily_user_code_intervals_item_id = test_utils.get_item_id(123, item_names.daily_user_code_intervals)

-- add several pin codes to test restrictions
set_item_dict_value({item_id = pin_codes_item_id, key = "1", value = {code = "1111", name = "test1"}})
set_item_dict_value({item_id = pin_codes_item_id, key = "2", value = {code = "2222", name = "test2"}})
set_item_dict_value({item_id = pin_codes_item_id, key = "3", value = {code = "3333", name = "test3"}})
set_item_dict_value({item_id = pin_codes_item_id, key = "4", value = {code = "4444", name = "test4"}})

tests.assert_eq(
    core.get_item(pin_codes_item_id).value, 
    {
        ["1"] = {code = "1111", name = "test1"},
        ["2"] = {code = "2222", name = "test2"},
        ["3"] = {code = "3333", name = "test3"},
        ["4"] = {code = "4444", name = "test4"},
    }, 
    "check core item value"
)

-- single interval
do
    set_item_dict_value(
        {
            item_id = daily_user_code_intervals_item_id, 
            key = "1", 
            value = {
                {
                    startDateTime = "2021-01-01T01:01:01",
                    stopDateTime = "2021-02-30T23:59:59"
                }
            }
        }
    )
    
    tests.assert_eq(
        core.get_item(daily_user_code_intervals_item_id).value, 
        {
            ["1"] = {
                {
                    startDateTime = "2021-01-01T01:01:01",
                    stopDateTime = "2021-02-30T23:59:59"
                }
            }
        }, 
        "check core item value"
    )
end

-- several intervals
do
    set_item_dict_value(
        {
            item_id = daily_user_code_intervals_item_id, 
            key = "2", 
            value = {
                {
                    startDateTime = "2021-03-01T01:01:01",
                    stopDateTime = "2021-04-30T23:59:59"
                },
                {
                    startDateTime = "2021-05-01T01:01:01",
                    stopDateTime = "2021-06-30T23:59:59"
                }
            }
        }
    )
    
    tests.assert_eq(
        core.get_item(daily_user_code_intervals_item_id).value, 
        {
            ["1"] = {
                {
                    startDateTime = "2021-01-01T01:01:01",
                    stopDateTime = "2021-02-30T23:59:59"
                }
            },
            ["2"] = {
                {
                    startDateTime = "2021-03-01T01:01:01",
                    stopDateTime = "2021-04-30T23:59:59"
                },
                {
                    startDateTime = "2021-05-01T01:01:01",
                    stopDateTime = "2021-06-30T23:59:59"
                }
            }
        }, 
        "check core item value"
    )
end

-- try set interval to unknown pin code
do
    set_item_dict_value(
        {
            item_id = daily_user_code_intervals_item_id, 
            key = "5", 
            value = {
                {
                    startDateTime = "2021-05-01T01:01:01",
                    stopDateTime = "2021-06-30T23:59:59"
                }
            }
        }
    )
    
    tests.assert_eq(
        core.get_item(daily_user_code_intervals_item_id).value, 
        {
            ["1"] = {
                {
                    startDateTime = "2021-01-01T01:01:01",
                    stopDateTime = "2021-02-30T23:59:59"
                }
            },
            ["2"] = {
                {
                    startDateTime = "2021-03-01T01:01:01",
                    stopDateTime = "2021-04-30T23:59:59"
                },
                {
                    startDateTime = "2021-05-01T01:01:01",
                    stopDateTime = "2021-06-30T23:59:59"
                }
            }
        }, 
        "check core item value"
    )
end

-- change interval
do
    set_item_dict_value(
        {
            item_id = daily_user_code_intervals_item_id, 
            key = "1", 
            value = {
                {
                    startDateTime = "2019-01-01T01:01:01",
                    stopDateTime = "2019-02-30T23:59:59"
                }
            }
        }
    )
    
    tests.assert_eq(
        core.get_item(daily_user_code_intervals_item_id).value, 
        {
            ["1"] = {
                {
                    startDateTime = "2019-01-01T01:01:01",
                    stopDateTime = "2019-02-30T23:59:59"
                }
            },
            ["2"] = {
                {
                    startDateTime = "2021-03-01T01:01:01",
                    stopDateTime = "2021-04-30T23:59:59"
                },
                {
                    startDateTime = "2021-05-01T01:01:01",
                    stopDateTime = "2021-06-30T23:59:59"
                }
            }
        }, 
        "check core item value"
    )
end

-- remove interval
do
    rem_item_dict_value({item_id = daily_user_code_intervals_item_id, key = "1"})
    
    tests.assert_eq(
        core.get_item(daily_user_code_intervals_item_id).value, 
        {
            ["2"] = {
                {
                    startDateTime = "2021-03-01T01:01:01",
                    stopDateTime = "2021-04-30T23:59:59"
                },
                {
                    startDateTime = "2021-05-01T01:01:01",
                    stopDateTime = "2021-06-30T23:59:59"
                }
            }
        }, 
        "check core item value"
    )
end

-- remove interval on pin code removed
do
    rem_item_dict_value({item_id = pin_codes_item_id, key = "2"})
    
    tests.assert_eq(
        core.get_item(daily_user_code_intervals_item_id).value, 
        {}, 
        "check core item value"
    )
end

-- clear intervals
do
    set_item_dict_value(
        {
            item_id = daily_user_code_intervals_item_id, 
            key = "1", 
            value = {
                {
                    startDateTime = "2021-01-01T01:01:01",
                    stopDateTime = "2021-02-30T23:59:59"
                }
            }
        }
    )

    set_item_dict_value(
        {
            item_id = daily_user_code_intervals_item_id, 
            key = "3", 
            value = {
                {
                    startDateTime = "2021-03-01T01:01:01",
                    stopDateTime = "2021-04-30T23:59:59"
                },
                {
                    startDateTime = "2021-05-01T01:01:01",
                    stopDateTime = "2021-06-30T23:59:59"
                }
            }
        }
    )
    
    tests.assert_eq(
        core.get_item(daily_user_code_intervals_item_id).value, 
        {
            ["1"] = {
                {
                    startDateTime = "2021-01-01T01:01:01",
                    stopDateTime = "2021-02-30T23:59:59"
                }
            },
            ["3"] = {
                {
                    startDateTime = "2021-03-01T01:01:01",
                    stopDateTime = "2021-04-30T23:59:59"
                },
                {
                    startDateTime = "2021-05-01T01:01:01",
                    stopDateTime = "2021-06-30T23:59:59"
                }
            }
        }, 
        "check core item value"
    )
    
    clear_item_dict({item_id = daily_user_code_intervals_item_id})
    
    tests.assert_eq(
        core.get_item(daily_user_code_intervals_item_id).value, 
        {}, 
        "check core item value"
    )
end

-- clear pin codes
do
    set_item_dict_value(
        {
            item_id = daily_user_code_intervals_item_id, 
            key = "1", 
            value = {
                {
                    startDateTime = "2021-01-01T01:01:01",
                    stopDateTime = "2021-02-30T23:59:59"
                }
            }
        }
    )

    set_item_dict_value(
        {
            item_id = daily_user_code_intervals_item_id, 
            key = "3", 
            value = {
                {
                    startDateTime = "2021-03-01T01:01:01",
                    stopDateTime = "2021-04-30T23:59:59"
                },
                {
                    startDateTime = "2021-05-01T01:01:01",
                    stopDateTime = "2021-06-30T23:59:59"
                }
            }
        }
    )
    
    tests.assert_eq(
        core.get_item(daily_user_code_intervals_item_id).value, 
        {
            ["1"] = {
                {
                    startDateTime = "2021-01-01T01:01:01",
                    stopDateTime = "2021-02-30T23:59:59"
                }
            },
            ["3"] = {
                {
                    startDateTime = "2021-03-01T01:01:01",
                    stopDateTime = "2021-04-30T23:59:59"
                },
                {
                    startDateTime = "2021-05-01T01:01:01",
                    stopDateTime = "2021-06-30T23:59:59"
                }
            }
        }, 
        "check core item value"
    )
    
    clear_item_dict({item_id = pin_codes_item_id})
    
    tests.assert_eq(
        core.get_item(daily_user_code_intervals_item_id).value, 
        {}, 
        "check core item value"
    )
end
