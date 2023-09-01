#include <znet/variant.hpp>

#include <gtest/gtest.h>

#include <memory>

namespace znet {
namespace ut {

namespace {

template <size_t>
struct NumberedType {};

template <size_t...>
struct NumberedTypeHelper;

template <size_t A, size_t... Other>
struct NumberedTypeHelper<A, Other...> : NumberedTypeHelper<A - 1, A, Other...> {
    using Variant = typename NumberedTypeHelper<A - 1, A, Other...>::Variant;
};

template <size_t... Other>
struct NumberedTypeHelper<0, Other...> {
    using Variant = utils::variant<NumberedType<Other>...>;
};

void fakeIntDeleter( int* value ) {
    *value = 0;
};

using TestIntUPtr = std::unique_ptr<int, void ( * )( int* )>;

using TestValue = utils::variant<int, float, TestIntUPtr, std::string, bool>;

}  // namespace

TEST( VariantTestSuite, compare ) {
    int testInt[2] = { 1, 2 };

    std::vector<TestValue> values1;
    std::vector<TestValue> values2;

    values1.push_back( 1 );
    values1.push_back( 1.f );
    values1.push_back( TestIntUPtr( testInt, fakeIntDeleter ) );
    values1.push_back( std::string( "1" ) );
    values1.push_back( false );

    values2.push_back( 2 );
    values2.push_back( 2.f );
    values2.push_back( TestIntUPtr( testInt + 1, fakeIntDeleter ) );
    values2.push_back( std::string( "2" ) );
    values2.push_back( true );

    for( size_t i = 0; i < values1.size(); ++i ) {
        EXPECT_EQ( values1[i], values1[i] );
        EXPECT_LT( values1[i], values2[i] );

        for( size_t j = 0; j < i; ++j ) {
            EXPECT_LT( values1[j], values1[i] );
            EXPECT_LT( values2[j], values1[i] );
        }
    }
}

TEST( VariantTestSuite, destruction ) {
    int testInt = 1;

    {
        auto value = TestValue( TestIntUPtr( &testInt, fakeIntDeleter ) );
        EXPECT_EQ( testInt, 1 );
    }

    EXPECT_EQ( testInt, 0 );
}

TEST( VariantTestSuite, getPtr ) {
    auto intVal = TestValue( 1 );
    auto floatVal = TestValue( 1.f );
    auto strVal = TestValue( std::string( "1" ) );

    EXPECT_EQ( intVal.getPtr<float>(), nullptr );
    EXPECT_EQ( intVal.getPtr<std::string>(), nullptr );

    EXPECT_EQ( floatVal.getPtr<int>(), nullptr );
    EXPECT_EQ( floatVal.getPtr<std::string>(), nullptr );

    EXPECT_EQ( strVal.getPtr<float>(), nullptr );
    EXPECT_EQ( strVal.getPtr<int>(), nullptr );

    ASSERT_NE( intVal.getPtr<int>(), nullptr );
    EXPECT_EQ( *intVal.getPtr<int>(), 1 );
    *intVal.getPtr<int>() = 2;
    EXPECT_EQ( *intVal.getPtr<int>(), 2 );

    ASSERT_NE( floatVal.getPtr<float>(), nullptr );
    EXPECT_EQ( *floatVal.getPtr<float>(), 1.f );
    *floatVal.getPtr<float>() = 2.f;
    EXPECT_EQ( *floatVal.getPtr<float>(), 2.f );

    ASSERT_NE( strVal.getPtr<std::string>(), nullptr );
    EXPECT_EQ( *strVal.getPtr<std::string>(), "1" );
    *strVal.getPtr<std::string>() = "2";
    EXPECT_EQ( *strVal.getPtr<std::string>(), "2" );
}

TEST( VariantTestSuite, constGetPtr ) {
    const TestValue intVal( 1 );
    const TestValue floatVal( 1.f );
    const TestValue strVal( std::string( "1" ) );

    EXPECT_EQ( intVal.getPtr<float>(), nullptr );
    EXPECT_EQ( intVal.getPtr<std::string>(), nullptr );

    EXPECT_EQ( floatVal.getPtr<int>(), nullptr );
    EXPECT_EQ( floatVal.getPtr<std::string>(), nullptr );

    EXPECT_EQ( strVal.getPtr<float>(), nullptr );
    EXPECT_EQ( strVal.getPtr<int>(), nullptr );

    ASSERT_NE( intVal.getPtr<int>(), nullptr );
    EXPECT_EQ( *intVal.getPtr<int>(), 1 );

    ASSERT_NE( floatVal.getPtr<float>(), nullptr );
    EXPECT_EQ( *floatVal.getPtr<float>(), 1.f );

    ASSERT_NE( strVal.getPtr<std::string>(), nullptr );
    EXPECT_EQ( *strVal.getPtr<std::string>(), "1" );
}

TEST( VariantTestSuite, visit ) {
    int testInt[2] = { 1, 2 };

    TestValue intValue( 1 );
    TestValue floatValue( 2.f );
    TestValue intPtrValue( TestIntUPtr( testInt, fakeIntDeleter ) );
    TestValue stringValue( std::string( "1" ) );
    TestValue boolValue( true );

    {
        auto refOverloads = visitors::makeOverloads( []( int ) -> std::string { return "int"; },
                                                     []( float ) { return "float"; },
                                                     []( TestIntUPtr& ) { return "ptr"; },
                                                     []( std::string& ) { return "str"; },
                                                     []( bool ) { return "bool"; } );

        EXPECT_EQ( visitors::visit( refOverloads, intValue ), "int" );
        EXPECT_EQ( visitors::visit( refOverloads, floatValue ), "float" );
        EXPECT_EQ( visitors::visit( refOverloads, intPtrValue ), "ptr" );
        EXPECT_EQ( visitors::visit( refOverloads, stringValue ), "str" );
        EXPECT_EQ( visitors::visit( refOverloads, boolValue ), "bool" );
    }

    {
        auto crefOverloads = visitors::makeOverloads( []( int ) -> std::string { return "int"; },
                                                      []( float ) { return "float"; },
                                                      []( const TestIntUPtr& ) { return "ptr"; },
                                                      []( const std::string& ) { return "str"; },
                                                      []( bool ) { return "bool"; } );

        EXPECT_EQ( visitors::visit( crefOverloads, const_cast<const TestValue&>( intValue ) ), "int" );
        EXPECT_EQ( visitors::visit( crefOverloads, const_cast<const TestValue&>( floatValue ) ), "float" );
        EXPECT_EQ( visitors::visit( crefOverloads, const_cast<const TestValue&>( intPtrValue ) ), "ptr" );
        EXPECT_EQ( visitors::visit( crefOverloads, const_cast<const TestValue&>( stringValue ) ), "str" );
        EXPECT_EQ( visitors::visit( crefOverloads, const_cast<const TestValue&>( boolValue ) ), "bool" );
    }

    {
        auto rvalueOverloads = visitors::makeOverloads( []( int ) -> std::string { return "int"; },
                                                        []( float ) { return "float"; },
                                                        []( TestIntUPtr ) { return "ptr"; },
                                                        []( std::string ) { return "str"; },
                                                        []( bool ) { return "bool"; } );

        EXPECT_EQ( visitors::visit( rvalueOverloads, std::move( intValue ) ), "int" );
        EXPECT_EQ( visitors::visit( rvalueOverloads, std::move( floatValue ) ), "float" );
        EXPECT_EQ( visitors::visit( rvalueOverloads, std::move( intPtrValue ) ), "ptr" );
        EXPECT_EQ( visitors::visit( rvalueOverloads, std::move( stringValue ) ), "str" );
        EXPECT_EQ( visitors::visit( rvalueOverloads, std::move( boolValue ) ), "bool" );

        EXPECT_NE( intValue.getPtr<int>(), nullptr );
        EXPECT_NE( floatValue.getPtr<float>(), nullptr );
        EXPECT_NE( intPtrValue.getPtr<TestIntUPtr>(), nullptr );
        EXPECT_NE( stringValue.getPtr<std::string>(), nullptr );
        EXPECT_NE( boolValue.getPtr<bool>(), nullptr );
    }
}

TEST( VariantTestSuite, multiVisit ) {
    using V1 = utils::variant<int, std::string>;
    using V2 = utils::variant<float, int, std::string>;

    auto overloads = visitors::makeOverloads(
        []( int a, int b ) {
            EXPECT_EQ( a, 1 );
            EXPECT_EQ( b, 2 );
            return "int+int";
        },
        []( int, float ) { return "int+float"; },
        []( int, const std::string& ) { return "int+str"; },
        []( const std::string&, int ) { return "str+int"; },
        []( const std::string&, float ) { return "str+float"; },
        []( const std::string& a, const std::string& b ) {
            EXPECT_EQ( a, "1" );
            EXPECT_EQ( b, "2" );
            return "str+str";
        } );

    EXPECT_EQ( visitors::visit( overloads, V1( 1 ), V2( 2 ) ), std::string( "int+int" ) );
    EXPECT_EQ( visitors::visit( overloads, V1( 1 ), V2( 2.f ) ), std::string( "int+float" ) );
    EXPECT_EQ( visitors::visit( overloads, V1( 1 ), V2( std::string() ) ), std::string( "int+str" ) );

    EXPECT_EQ( visitors::visit( overloads, V1( std::string() ), V2( 2 ) ), std::string( "str+int" ) );
    EXPECT_EQ( visitors::visit( overloads, V1( std::string() ), V2( 2.f ) ), std::string( "str+float" ) );
    EXPECT_EQ( visitors::visit( overloads, V1( std::string( "1" ) ), V2( std::string( "2" ) ) ),
               std::string( "str+str" ) );
}

TEST( VariantTestSuite, index ) {
    int testInt[2] = { 1, 2 };

    TestValue intValue( 1 );
    TestValue floatValue( 2.f );
    TestValue intPtrValue( TestIntUPtr( testInt, fakeIntDeleter ) );
    TestValue stringValue( std::string( "1" ) );
    TestValue boolValue( true );

    EXPECT_EQ( intValue.getIndex(), 0 );
    EXPECT_EQ( floatValue.getIndex(), 1 );
    EXPECT_EQ( intPtrValue.getIndex(), 2 );
    EXPECT_EQ( stringValue.getIndex(), 3 );
    EXPECT_EQ( boolValue.getIndex(), 4 );
}

TEST( VariantTestSuite, assignment ) {
    // the same type
    {
        auto ptr1 = std::make_shared<int>( 0 );
        auto ptr2 = std::make_shared<int>( 0 );
        utils::variant<int, std::shared_ptr<int> > value( ptr1 );

        EXPECT_EQ( ptr1.use_count(), 2 );

        value = ptr2;

        EXPECT_EQ( ptr1.use_count(), 1 );
        EXPECT_EQ( ptr2.use_count(), 2 );

        value = std::move( ptr1 );

        EXPECT_EQ( ptr1.use_count(), 0 );
        EXPECT_EQ( ptr2.use_count(), 1 );
        EXPECT_EQ( value.get<std::shared_ptr<int> >().use_count(), 1 );
    }

    // different types
    {
        auto ptr1 = std::make_shared<int>( 0 );
        auto ptr2 = std::make_shared<float>( 0 );
        utils::variant<std::shared_ptr<int>, std::shared_ptr<float> > value( ptr1 );

        EXPECT_EQ( ptr1.use_count(), 2 );

        value = ptr2;

        EXPECT_EQ( ptr1.use_count(), 1 );
        EXPECT_EQ( ptr2.use_count(), 2 );

        value = std::move( ptr1 );

        EXPECT_EQ( ptr1.use_count(), 0 );
        EXPECT_EQ( value.get<std::shared_ptr<int> >().use_count(), 1 );
        EXPECT_EQ( ptr2.use_count(), 1 );
    }

    // assign other multivalue
    {
        auto ptr1 = std::make_shared<int>( 0 );
        auto ptr2 = std::make_shared<float>( 0 );
        utils::variant<std::shared_ptr<int>, std::shared_ptr<float> > value1( ptr1 );
        utils::variant<std::shared_ptr<int>, std::shared_ptr<float> > value2( ptr2 );

        EXPECT_EQ( ptr1.use_count(), 2 );

        value1 = value2;

        EXPECT_EQ( ptr1.use_count(), 1 );
        EXPECT_EQ( ptr2.use_count(), 3 );

        value2 = ptr1;
        value2 = std::move( value1 );

        EXPECT_EQ( ptr1.use_count(), 1 );
        EXPECT_EQ( ptr2.use_count(), 2 );

        EXPECT_NE( value1.getPtr<std::shared_ptr<float> >(), nullptr );
        EXPECT_EQ( value1.get<std::shared_ptr<float> >().use_count(), 0 );
    }
}

TEST( VariantTestSuite, contruction ) {
    auto ptr1 = std::make_shared<int>( 0 );
    auto ptr2 = std::make_shared<float>( 0 );

    {
        utils::variant<std::shared_ptr<int>, std::shared_ptr<float> > value1( std::move( ptr1 ) );
        utils::variant<std::shared_ptr<int>, std::shared_ptr<float> > value2( ptr2 );

        EXPECT_EQ( ptr1.use_count(), 0 );
        EXPECT_EQ( ptr2.use_count(), 2 );

        EXPECT_EQ( value2.get<std::shared_ptr<float> >().use_count(), 2 );

        utils::variant<std::shared_ptr<int>, std::shared_ptr<float> > value3( value2 );
        utils::variant<std::shared_ptr<int>, std::shared_ptr<float> > value4( std::move( value2 ) );

        EXPECT_NE( value2.getPtr<std::shared_ptr<float> >(), nullptr );
        EXPECT_EQ( value2.get<std::shared_ptr<float> >().use_count(), 0 );

        EXPECT_EQ( ptr2.use_count(), 3 );
    }

    EXPECT_EQ( ptr2.use_count(), 1 );
}

TEST( VariantTestSuite, defaultConstructor ) {
    struct Test {
        int value = 100500;
    };

    utils::variant<Test, int> value;

    EXPECT_EQ( value.getIndex(), 0 );
    EXPECT_EQ( value.get<Test>().value, 100500 );
}

TEST( VariantTestSuite, muchTypes ) {
    using Variant255 = typename NumberedTypeHelper<255>::Variant;
    using Variant256 = typename NumberedTypeHelper<256>::Variant;
    EXPECT_TRUE( ( std::is_same<typename Variant255::Index, uint8_t>::value ) );
    EXPECT_TRUE( ( std::is_same<typename Variant256::Index, uint16_t>::value ) );

    EXPECT_TRUE( visitors::visit( visitors::makeOverloads( []( NumberedType<255> ) { return true; },
                                                           visitors::makeDefaultOverload( []() { return false; } ) ),
                                  Variant255( NumberedType<255>() ) ) );

    EXPECT_TRUE( visitors::visit( visitors::makeOverloads( []( NumberedType<256> ) { return true; },
                                                           visitors::makeDefaultOverload( []() { return false; } ) ),
                                  Variant256( NumberedType<256>() ) ) );

    using Variant127 = typename NumberedTypeHelper<127>::Variant;
    using Variant128 = typename NumberedTypeHelper<128>::Variant;
    EXPECT_TRUE( visitors::visit( visitors::makeOverloads( []( NumberedType<127> ) { return true; },
                                                           visitors::makeDefaultOverload( []() { return false; } ) ),
                                  Variant127( NumberedType<127>() ) ) );

    EXPECT_TRUE( visitors::visit( visitors::makeOverloads( []( NumberedType<128> ) { return true; },
                                                           visitors::makeDefaultOverload( []() { return false; } ) ),
                                  Variant128( NumberedType<128>() ) ) );

    using Variant1 = typename NumberedTypeHelper<1>::Variant;
    EXPECT_TRUE( visitors::visit( visitors::makeOverloads( []( NumberedType<1> ) { return true; },
                                                           visitors::makeDefaultOverload( []() { return false; } ) ),
                                  Variant1( NumberedType<1>() ) ) );
}

}  // namespace ut
}  // namespace znet
