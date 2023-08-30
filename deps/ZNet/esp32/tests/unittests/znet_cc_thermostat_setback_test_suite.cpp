#include "MainFixture.h"

#include <znet/znet_node.hpp>
#include <znet/cc_0x47_thermostat_setback.hpp>
#include <znet/converters/thermostat_setback_converter.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "znet_node_requests_managing_fixture.h"
#include "znet_fc_send_data_mock.h"

extern "C" {
#include <znet_lib_cc.h>
}

namespace znet {

namespace ut {

using ::testing::_;
using ::testing::ElementsAre;
using ::testing::Invoke;
using ::testing::MockFunction;

class ZCcThermostatSetbackTestSuite : public MainFixture, public NodeRequestsManagingFixture, public FcSendDataMock {
public:
};

TEST_F( ZCcThermostatSetbackTestSuite, interview ) {
    MockFunction<void( RequestFailTypeOpt )> callback;
    auto cc = ensureNodeAndCc<ZCc0x47ThermostatSetback>( 1, 0, 1 );

    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_FALSE( cc->interview( callback.AsStdFunction(), {} ) );
    EXPECT_FALSE( cc->hasInterviewDone() );
}

TEST_F( ZCcThermostatSetbackTestSuite, setSetback ) {
    auto cc = ensureNodeAndCc<ZCc0x47ThermostatSetback>( 1, 2, 1 );

    const std::vector<uint8_t> expectedSentData = {
        COMMAND_CLASS_THERMOSTAT_SETBACK, THERMOSTAT_SETBACK_SET, 0x02, 101
    };

    EXPECT_CALL( *this, fcSendData( 1, 2, expectedSentData, false, false, Encapsulation_None ) );

    EXPECT_TRUE( cc->setSetback(
        thermostat_setback::OverrideType::PermanentOverride, thermostat_setback::Mode::CustomValue, 10.1f ) );
}

TEST_F( ZCcThermostatSetbackTestSuite, requestValue ) {
    MockFunction<void( RequestFailTypeOpt )> callback;
    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    auto cc = ensureNodeAndCc<ZCc0x47ThermostatSetback>( 1, 2, 1 );

    getRequestsManager().setUseFakeRequests( false );

    const std::vector<uint8_t> expectedSentData = { COMMAND_CLASS_THERMOSTAT_SETBACK, THERMOSTAT_SETBACK_GET };

    EXPECT_CALL( *this, fcSendData( 1, 2, expectedSentData, true, false, Encapsulation_None ) );
    EXPECT_TRUE( cc->requestValue( createRequestValue( callback ) ) );

    EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );
    getRequestsManager().onRespond( ThermostatSetbackRequest( 1, 2 ), 2, 0x15 );
}

TEST( ZCcThermostatSetbackConverterTestSuite, toRawValue ) {
    using namespace thermostat_setback;

    // custom value
    {
        EXPECT_EQ( toRawValue( Mode::CustomValue, 13.1f ), 120 );
        EXPECT_EQ( toRawValue( Mode::CustomValue, 12.05f ), 120 );
        EXPECT_EQ( toRawValue( Mode::CustomValue, 11.95f ), 120 );

        EXPECT_EQ( toRawValue( Mode::CustomValue, -12.85f ), -128 );
        EXPECT_EQ( toRawValue( Mode::CustomValue, -12.751f ), -128 );
        EXPECT_EQ( toRawValue( Mode::CustomValue, -14.f ), -128 );

        EXPECT_EQ( toRawValue( Mode::CustomValue, 11.949f ), 119 );
        EXPECT_EQ( toRawValue( Mode::CustomValue, -12.75f ), -127 );
        EXPECT_EQ( toRawValue( Mode::CustomValue, 0.f ), 0 );
        EXPECT_EQ( toRawValue( Mode::CustomValue, 0.15f ), 2 );
    }

    // specific modes
    {
        EXPECT_EQ( toRawValue( Mode::FrostProtection, 100 ), 121 );
        EXPECT_EQ( toRawValue( Mode::FrostProtection, {} ), 121 );

        EXPECT_EQ( toRawValue( Mode::EnergySavingMode, 100 ), 122 );
        EXPECT_EQ( toRawValue( Mode::EnergySavingMode, {} ), 122 );
    }

    // unknown mode
    {
        EXPECT_EQ( toRawValue( Mode::Unknown, 100 ), 100 );
        EXPECT_EQ( toRawValue( Mode::Unknown, 100.8f ), 100 );
        EXPECT_EQ( toRawValue( Mode::Unknown, {} ), 0 );
    }
}

TEST( ZCcThermostatSetbackConverterTestSuite, fromRawValue ) {
    using namespace thermostat_setback;

    // custom value
    {
        EXPECT_EQ( fromRawValue( 120 ), std::make_pair( Mode::CustomValue, SetbackValueOpt( 12.f ) ) );
        EXPECT_EQ( fromRawValue( -128 ), std::make_pair( Mode::CustomValue, SetbackValueOpt( -12.8f ) ) );
        EXPECT_EQ( fromRawValue( -127 ), std::make_pair( Mode::CustomValue, SetbackValueOpt( -12.7f ) ) );
        EXPECT_EQ( fromRawValue( 0 ), std::make_pair( Mode::CustomValue, SetbackValueOpt( 0.f ) ) );
    }

    // specific modes
    {
        EXPECT_EQ( fromRawValue( 121 ), std::make_pair( Mode::FrostProtection, SetbackValueOpt{} ) );
        EXPECT_EQ( fromRawValue( 122 ), std::make_pair( Mode::EnergySavingMode, SetbackValueOpt{} ) );
    }

    // unknown mode
    {
        EXPECT_EQ( fromRawValue( 123 ), std::make_pair( Mode::Unknown, SetbackValueOpt( 123.f ) ) );
        EXPECT_EQ( fromRawValue( 127 ), std::make_pair( Mode::Unknown, SetbackValueOpt( 127.f ) ) );
    }
}

TEST( ZCcThermostatSetbackConverterTestSuite, modeToString ) {
    using namespace thermostat_setback;

    EXPECT_EQ( modeToString( Mode::FrostProtection ), std::string( "frost_protection" ) );
    EXPECT_EQ( modeToString( Mode::CustomValue ), std::string( "custom_value" ) );
    EXPECT_EQ( modeToString( Mode::EnergySavingMode ), std::string( "energy_saving_mode" ) );
    EXPECT_EQ( modeToString( Mode::Unknown ), std::string( "unknown" ) );
    EXPECT_EQ( modeToString( Mode( 0xFF ) ), nullptr );
}

TEST( ZCcThermostatSetbackConverterTestSuite, stringToMode ) {
    using namespace thermostat_setback;

    EXPECT_EQ( stringToMode( "frost_protection" ), ModeOpt( Mode::FrostProtection ) );
    EXPECT_EQ( stringToMode( "unknown" ), ModeOpt( Mode::Unknown ) );
    EXPECT_EQ( stringToMode( "custom_value" ), ModeOpt( Mode::CustomValue ) );
    EXPECT_EQ( stringToMode( "energy_saving_mode" ), ModeOpt( Mode::EnergySavingMode ) );
    EXPECT_EQ( stringToMode( "invalid" ), ModeOpt() );
}

TEST( ZCcThermostatSetbackConverterTestSuite, overrideTypeToString ) {
    using namespace thermostat_setback;

    EXPECT_EQ( overrideTypeToString( OverrideType::NoOverride ), std::string( "no_override" ) );
    EXPECT_EQ( overrideTypeToString( OverrideType::TemporaryOverride ), std::string( "temporary_override" ) );
    EXPECT_EQ( overrideTypeToString( OverrideType::PermanentOverride ), std::string( "permanent_override" ) );
    EXPECT_EQ( overrideTypeToString( OverrideType::Unknown ), std::string( "unknown" ) );
    EXPECT_EQ( overrideTypeToString( OverrideType( 0xFF ) ), nullptr );
}

TEST( ZCcThermostatSetbackConverterTestSuite, stringToOverrideType ) {
    using namespace thermostat_setback;

    EXPECT_EQ( stringToOverrideType( "no_override" ), OverrideTypeOpt( OverrideType::NoOverride ) );
    EXPECT_EQ( stringToOverrideType( "temporary_override" ), OverrideTypeOpt( OverrideType::TemporaryOverride ) );
    EXPECT_EQ( stringToOverrideType( "permanent_override" ), OverrideTypeOpt( OverrideType::PermanentOverride ) );
    EXPECT_EQ( stringToOverrideType( "unknown" ), OverrideTypeOpt( OverrideType::Unknown ) );
    EXPECT_EQ( stringToOverrideType( "invalid" ), OverrideTypeOpt() );
}

}  // namespace ut
}  // namespace znet
