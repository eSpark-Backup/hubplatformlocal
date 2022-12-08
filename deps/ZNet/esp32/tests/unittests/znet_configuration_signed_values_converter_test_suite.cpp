#include <znet/converters/signed_configuration_parameter_converter.hpp>
#include <znet/enums/configuration_format.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

namespace znet {
namespace ut {

TEST( ZConfigurationSignedValuesConverterTestSuite, unsignedFromats ) {
    ConfigurationFormat formats[] = {
        ConfigurationFormat::UnsignedInteger, ConfigurationFormat::Enumerated, ConfigurationFormat::BitField
    };

    std::vector<std::pair<uint8_t, uint32_t> > sizedValues = { { 1, 170 }, { 2, 43690 }, { 4, 2863311530 } };

    for( auto format : formats ) {
        for( const auto& sizedValue : sizedValues ) {
            ASSERT_EQ(
                sizedValue.second,
                signed_configuration_parameter_converter::toUiValue( sizedValue.second, sizedValue.first, format ) );
            ASSERT_EQ(
                sizedValue.second,
                signed_configuration_parameter_converter::toZNetValue( sizedValue.second, sizedValue.first, format ) );
        }
    }
}

TEST( ZConfigurationSignedValuesConverterTestSuite, signedFromat ) {
    ConfigurationFormat format = ConfigurationFormat::SignedInteger;

    auto doTest = [format]( uint32_t znetValue, int64_t uiValue, uint8_t size ) {
        ASSERT_EQ( uiValue, signed_configuration_parameter_converter::toUiValue( znetValue, size, format ) );
        ASSERT_EQ( znetValue, signed_configuration_parameter_converter::toZNetValue( uiValue, size, format ) );
    };

    // Test size 1
    {
        uint8_t size = 1;

        doTest( 0x7F, 127, size );
        doTest( 0x02, 2, size );
        doTest( 0x01, 1, size );
        doTest( 0x00, 0, size );
        doTest( 0xFF, -1, size );
        doTest( 0xFE, -2, size );
        doTest( 0x80, -128, size );
    }

    // Test size 2
    {
        uint8_t size = 2;

        doTest( 0x7FFF, 32767, size );
        doTest( 0x02, 2, size );
        doTest( 0x01, 1, size );
        doTest( 0x00, 0, size );
        doTest( 0xFFFF, -1, size );
        doTest( 0xFFFE, -2, size );
        doTest( 0x8000, -32768, size );
    }

    // Test size 4
    {
        uint8_t size = 4;

        doTest( 0x7FFFFFFF, 2147483647, size );
        doTest( 0x02, 2, size );
        doTest( 0x01, 1, size );
        doTest( 0x00, 0, size );
        doTest( 0xFFFFFFFF, -1, size );
        doTest( 0xFFFFFFFE, -2, size );
        doTest( 0x80000000, -2147483648, size );
    }
}

}  // namespace ut
}  // namespace znet
