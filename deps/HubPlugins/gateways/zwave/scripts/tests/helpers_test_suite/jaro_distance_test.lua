local tests = require "tests"

local round = loadfile("HUB:zwave/scripts/helpers/round")

local jaro_distance_impl = loadfile("HUB:zwave/scripts/helpers/jaro_distance")
local function jaro_distance(a, b)
    return round(jaro_distance_impl(a, b), 2)
end


tests.assert_eq(jaro_distance("", ""), 1, "both empty words" )
tests.assert_eq(jaro_distance("test", ""), 0, "one empty words")
tests.assert_eq(jaro_distance("", "test"), 0, "one empty words")
tests.assert_eq(jaro_distance("aaaa", "bbbb"), 0, "different words")

tests.assert_eq(jaro_distance("Loans and Accounts", "Loans Accounts"), 0.88, "loss of word" )
tests.assert_eq(jaro_distance("loans and accounts", "loan and account"), 0.96, "small changes" )
tests.assert_eq(jaro_distance("loans and accounts", "accounts and loans"), 0.76, "rearrangement of words" )
tests.assert_eq(jaro_distance("fishing, \"camping\"; and 'forest", "fishing camping and forest"), 0.95, "punctuation" )
tests.assert_eq(jaro_distance("Loan Account and Dealing", "LOAN ACCOUNTS DEALINGS"), 0.41, "case" )
tests.assert_eq(jaro_distance("LoanAccountDealing", "Load, Account, Dealing"), 0.87, "spacing" )
