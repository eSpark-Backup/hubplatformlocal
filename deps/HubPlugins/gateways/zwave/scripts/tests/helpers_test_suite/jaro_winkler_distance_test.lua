local tests = require "tests"

local round = loadfile("HUB:zwave/scripts/helpers/round")

local jaro_winkler_distance_impl = loadfile("HUB:zwave/scripts/helpers/jaro_winkler_distance")
local function jaro_winkler_distance(a, b)
    return round(jaro_winkler_distance_impl(a, b), 2)
end


tests.assert_eq(jaro_winkler_distance("", ""), 1, "both empty words" )
tests.assert_eq(jaro_winkler_distance("test", ""), 0, "one empty words")
tests.assert_eq(jaro_winkler_distance("", "test"), 0, "one empty words")
tests.assert_eq(jaro_winkler_distance("aaaa", "bbbb"), 0, "different words")

tests.assert_eq(jaro_winkler_distance("Loans and Accounts", "Loans Accounts"), 0.93, "loss of word" )
tests.assert_eq(jaro_winkler_distance("loans and accounts", "loan and account"), 0.98, "small changes" )
tests.assert_eq(jaro_winkler_distance("loans and accounts", "accounts and loans"), 0.76, "rearrangement of words" )
tests.assert_eq(jaro_winkler_distance("fishing, \"camping\"; and 'forest", "fishing camping and forest"), 0.97, "punctuation" )
tests.assert_eq(jaro_winkler_distance("Loan Account and Dealing", "LOAN ACCOUNTS DEALINGS"), 0.41, "case" )
tests.assert_eq(jaro_winkler_distance("LoanAccountDealing", "Load, Account, Dealing"), 0.91, "spacing" )
