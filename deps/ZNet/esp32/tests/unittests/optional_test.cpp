#include <gtest/gtest.h>

#include "znet/optional.hpp"

namespace znet {
namespace ut {

TEST( Optional, ShouldBeEmpty_WhenDefaultConstructed ) {
    utils::optional<std::string> emptyOptional;
    ASSERT_FALSE( emptyOptional.has_value() );
}

TEST( Optional, ShouldBeEmpty_WhenInitWithEmptyOtherOptional ) {
    utils::optional<std::string> emptyOptional;

    utils::optional<std::string> optionalConstructedFromEmptyOther{ emptyOptional };
    ASSERT_FALSE( optionalConstructedFromEmptyOther.has_value() );

    utils::optional<std::string> defaultConstructedOptionalAssignedEmptyOther{};
    defaultConstructedOptionalAssignedEmptyOther = emptyOptional;
    ASSERT_FALSE( defaultConstructedOptionalAssignedEmptyOther.has_value() );
}

TEST( Optional, ShouldBeEmpty_WhenReset ) {
    const std::string initValue = "initial value";

    utils::optional<std::string> optionalConstructedFromValue{ initValue };
    optionalConstructedFromValue.reset();
    ASSERT_FALSE( optionalConstructedFromValue.has_value() );

    utils::optional<std::string> emptyOptional{};
    emptyOptional.reset();
    ASSERT_FALSE( emptyOptional.has_value() );
}

TEST( Optional, ShouldStoreValue_WhenInitWithValue ) {
    std::string initValue = "initial value";

    utils::optional<std::string> optionalConstructedFromValue{ initValue };
    ASSERT_TRUE( optionalConstructedFromValue.has_value() );
    ASSERT_EQ( initValue, *optionalConstructedFromValue );

    utils::optional<std::string> defaultConstructedOptionalAssignedValue;
    defaultConstructedOptionalAssignedValue = initValue;
    ASSERT_TRUE( defaultConstructedOptionalAssignedValue.has_value() );
    ASSERT_EQ( initValue, *defaultConstructedOptionalAssignedValue );
}

TEST( Optional, ShouldChangeValue_WhenAssignedOtherValue ) {
    const std::string initialValue = "initial value";
    const std::string changedValue = "changed value";

    utils::optional<std::string> optionalConstructedFromValue{ initialValue };
    optionalConstructedFromValue = changedValue;
    ASSERT_TRUE( optionalConstructedFromValue.has_value() );
    ASSERT_EQ( changedValue, *optionalConstructedFromValue );

    utils::optional<std::string> defaultConstructedOptionalAssignedValue;
    defaultConstructedOptionalAssignedValue = initialValue;
    defaultConstructedOptionalAssignedValue = changedValue;
    ASSERT_TRUE( defaultConstructedOptionalAssignedValue.has_value() );
    ASSERT_EQ( changedValue, *defaultConstructedOptionalAssignedValue );
}

TEST( Optional, ShouldStoreValue_WhenInitWithOtherOptional ) {
    const std::string initValue = "initial value";

    utils::optional<std::string> other{ initValue };

    utils::optional<std::string> optionalConstructedFromOther{ other };
    ASSERT_TRUE( optionalConstructedFromOther.has_value() );
    ASSERT_EQ( initValue, *optionalConstructedFromOther );

    utils::optional<std::string> defaultConstructedOptionalAssignedOther{};
    defaultConstructedOptionalAssignedOther = other;
    ASSERT_TRUE( defaultConstructedOptionalAssignedOther.has_value() );
    ASSERT_EQ( initValue, *defaultConstructedOptionalAssignedOther );
}

TEST( Optional, ShouldChangeValue_WhenAssignedOtherOptional ) {
    const std::string initialValue = "initial value";
    const std::string changedValue = "changed value";

    utils::optional<std::string> firstOptional{ initialValue };
    utils::optional<std::string> secondOptional{ changedValue };

    utils::optional<std::string> optionalConstructedFromOther{ firstOptional };
    optionalConstructedFromOther = secondOptional;
    ASSERT_TRUE( optionalConstructedFromOther.has_value() );
    ASSERT_EQ( changedValue, *optionalConstructedFromOther );

    utils::optional<std::string> defaultConstructedOptionalAssignedOther{};
    defaultConstructedOptionalAssignedOther = firstOptional;
    defaultConstructedOptionalAssignedOther = secondOptional;
    ASSERT_TRUE( defaultConstructedOptionalAssignedOther.has_value() );
    ASSERT_EQ( changedValue, *defaultConstructedOptionalAssignedOther );
}

TEST( Optional, ShouldReturnNullptr_WhenEmpty ) {
    utils::optional<std::string> emptyOptional{};
    ASSERT_EQ( emptyOptional.get(), nullptr );

    const utils::optional<std::string> emptyConstOptional{};
    ASSERT_EQ( emptyConstOptional.get(), nullptr );
}

TEST( Optional, ShouldAbortExecution_WhenDereferencingEmptyOptional ) {
    utils::optional<std::string> emptyOptional{};
    ASSERT_DEATH( *emptyOptional, ".*" ) << "operator*() on empty l-value optional";
    ASSERT_DEATH( emptyOptional->empty(), ".*" ) << "operator->() on empty l-value optional";

    const utils::optional<std::string> emptyConstOptional{};
    ASSERT_DEATH( *emptyConstOptional, ".*" ) << "operator*() on empty const l-value optional";
    ASSERT_DEATH( emptyConstOptional->empty(), ".*" ) << "operator->() on empty const l-value optional";

    ASSERT_DEATH( *utils::optional<std::string>{}, ".*" ) << "operator*() on empty r-value optional";
    ASSERT_DEATH( utils::optional<std::string> {}->empty(), ".*" ) << "operator->() on empty r-value optional";
}

TEST( Optional, ShouldReturnValue_WhenDereferencingNonEmptyOptional ) {
    const std::string initialValue = "initial value";

    utils::optional<std::string> optionalConstructedFromValue{ initialValue };
    ASSERT_EQ( initialValue, *optionalConstructedFromValue )
        << "operator*() on non const l-value optional constructed from value";
    ASSERT_EQ( initialValue, optionalConstructedFromValue->substr() )
        << "operator->() on non const l-value optional constructed from value";

    const utils::optional<std::string> constOptionalConstructedFromValue{ initialValue };
    ASSERT_EQ( initialValue, *constOptionalConstructedFromValue )
        << "operator*() on const l-value optional constructed from value";
    ASSERT_EQ( initialValue, constOptionalConstructedFromValue->substr() )
        << "operator->() on const l-value optional constructed from value";

    ASSERT_EQ( initialValue, *utils::optional<std::string>{ initialValue } )
        << "operator*() on r-value optional constructed from value";
    ASSERT_EQ( initialValue, utils::optional<std::string> { initialValue }->substr() )
        << "operator->() on r-value optional constructed from value";
}

}  // namespace ut
}  // namespace znet
