local test = require( "tests" )

local security_tag_helper = loadfile("HUB:zwave/scripts/helpers/security_tag_helper")()

test.assert_eq(security_tag_helper.add_security_tag("tag"), "tag.security_tag", "add tag")
test.assert_eq(security_tag_helper.add_security_tag(""),  ".security_tag", "add tag to empty string")
test.assert_eq(security_tag_helper.add_security_tag(nil), ".security_tag", "add tag to nil string")
test.assert_eq(security_tag_helper.add_security_tag("t.security_tag"), "t.security_tag", "add tag to already tagged")

test.assert_eq(security_tag_helper.remove_security_tag("tag.sec"), "tag.sec", "remove tag from str without security_tag")
test.assert_eq(security_tag_helper.remove_security_tag(nil), nil, "remove tag from nil")
test.assert_eq(security_tag_helper.remove_security_tag("x.security_tag"), "x", "remove tag from string with security_tag")
test.assert_eq(security_tag_helper.remove_security_tag(".security_tag"), nil, "remove tag from string that equals to security_tag")
