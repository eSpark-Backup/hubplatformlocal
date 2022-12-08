#include <znet/znet_node.hpp>
#include <znet/cc_0x43_thermostat_setpoint.hpp>
#include <znet/znet_utils.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "znet_node_requests_managing_fixture.h"
#include "fake_file.h"

namespace znet {

static bool operator==( const ZCc0x43ThermostatSetpoint::setpointsDescription_t_v5& a,
                        const ZCc0x43ThermostatSetpoint::setpointsDescription_t_v5& b ) {
    return a.type == b.type && a.minScale == b.minScale && a.maxScale == b.maxScale &&
           std::abs( a.minValue - b.minValue ) < std::numeric_limits<float>::epsilon() &&
           std::abs( a.maxValue - b.maxValue ) < std::numeric_limits<float>::epsilon() && a.size == b.size &&
           a.precision == b.precision;
}

namespace ut {

using ::testing::_;
using ::testing::ElementsAre;
using ::testing::Invoke;
using ::testing::MockFunction;

class ZCcThermostatSetpointTestSuite : public ::testing::Test, public NodeRequestsManagingFixture {
protected:
    static znet_thermostat_setpoint_report_t generateSimpleSetpointReport( uint8_t type ) {
        znet_thermostat_setpoint_report_t report{};
        report.type = 2;
        report.size = 4;
        report.precision = 1;

        return report;
    }

    static znet_thermostat_setpoint_report_t generateUnsupportedSetpointReport() {
        znet_thermostat_setpoint_report_t report{};
        report.type = 0;
        return report;
    }

    static void fillBitMap( uint8_t* bitMap, std::vector<uint8_t> bitNumbers ) {
        for( auto bitNumber : bitNumbers ) {
            znet::utils::setBit( bitMap, bitNumber );
        }
    }

    ZCc0x43ThermostatSetpoint::setpointsDescription_t_v5 generateSimpleDescription( uint8_t type ) {
        ZCc0x43ThermostatSetpoint::setpointsDescription_t_v5 desc;
        desc.type = type;
        desc.size = 4;
        desc.precision = 1;

        return desc;
    }

    bool& getIsRequested() {
        return m_isRequested;
    }

    static znet_thermostat_setpoint_capabilities_report_t generateSimpleCapabilitiesReport( uint8_t type ) {
        znet_thermostat_setpoint_capabilities_report_t capabilitiesReport{};
        capabilitiesReport.type = type;
        capabilitiesReport.max_value = 2120;
        capabilitiesReport.min_value = 0;
        capabilitiesReport.max_params = ( 1 << 5 ) | ( 1 << 3 );
        capabilitiesReport.min_params = ( 1 << 5 ) | ( 1 << 3 );
        return capabilitiesReport;
    }

    static znet_thermostat_setpoint_capabilities_report_t getCapabilities2() {
        znet_thermostat_setpoint_capabilities_report_t capabilitiesReport2{};
        capabilitiesReport2.type = 2;
        capabilitiesReport2.max_value = 10;
        capabilitiesReport2.min_value = 10;
        capabilitiesReport2.max_params = ( 1 << 5 ) | ( 2 << 3 );
        capabilitiesReport2.min_params = ( 2 << 5 ) | ( 1 << 3 );
        return capabilitiesReport2;
    }

    static znet_thermostat_setpoint_capabilities_report_t getCapabilities9() {
        znet_thermostat_setpoint_capabilities_report_t capabilitiesReport9{};
        capabilitiesReport9.type = 9;
        capabilitiesReport9.max_value = 200;
        capabilitiesReport9.min_value = 20;
        capabilitiesReport9.max_params = ( 2 << 5 ) | ( 3 << 3 );
        capabilitiesReport9.min_params = 2 << 5;
        return capabilitiesReport9;
    }

    bool m_isRequested;
};

TEST_F( ZCcThermostatSetpointTestSuite, interviewV1InterpritationA ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    MockFunction<void( RequestFailTypeOpt )> callback;

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_THERMOSTAT_SETPOINT, 1 );
    auto cc = node->getCommandClass<ZCc0x43ThermostatSetpoint>( 0 );

    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

    uint8_t bitmask[32] = {
        0,
    };
    fillBitMap( bitmask, { 1, 2, 3, 4, 5, 6 } );
    znet_thermostat_setpoint_supported_report_t report{};
    report.bit_mask = bitmask;
    report.stcount = 32;
    node->getRequestsManager().onRespond( ThermostatSetpointSupportedRequest( 1, 0 ), report );

    EXPECT_FALSE( cc->hasInterviewDone() );
    node->getRequestsManager().onRespond(
        ThermostatAmbiguousSetpointRequest( 1, 0, 0x01 ), generateSimpleSetpointReport( 0x01 ), getIsRequested() );

    EXPECT_FALSE( cc->hasInterviewDone() );
    node->getRequestsManager().onRespond(
        ThermostatAmbiguousSetpointRequest( 1, 0, 0x02 ), generateSimpleSetpointReport( 0x02 ), getIsRequested() );

    EXPECT_FALSE( cc->hasInterviewDone() );
    node->getRequestsManager().onRespond(
        ThermostatAmbiguousSetpointRequest( 1, 0, 0x07 ), generateSimpleSetpointReport( 0x07 ), getIsRequested() );

    EXPECT_FALSE( cc->hasInterviewDone() );
    node->getRequestsManager().onRespond(
        ThermostatAmbiguousSetpointRequest( 1, 0, 0x08 ), generateSimpleSetpointReport( 0x08 ), getIsRequested() );

    EXPECT_FALSE( cc->hasInterviewDone() );
    node->getRequestsManager().onRespond(
        ThermostatAmbiguousSetpointRequest( 1, 0, 0x09 ), generateSimpleSetpointReport( 0x09 ), getIsRequested() );

    EXPECT_FALSE( cc->hasInterviewDone() );
    EXPECT_CALL( callback, Call( RequestFailTypeOpt() ) ).Times( 1 );
    node->getRequestsManager().onRespond(
        ThermostatAmbiguousSetpointRequest( 1, 0, 0x0A ), generateSimpleSetpointReport( 0x0A ), getIsRequested() );

    EXPECT_TRUE( cc->hasInterviewDone() );

    EXPECT_THAT( cc->getDescription(),
                 ElementsAre( generateSimpleDescription( 0x01 ),
                              generateSimpleDescription( 0x02 ),
                              generateSimpleDescription( 0x07 ),
                              generateSimpleDescription( 0x08 ),
                              generateSimpleDescription( 0x09 ),
                              generateSimpleDescription( 0x0A ) ) );
}

TEST_F( ZCcThermostatSetpointTestSuite, interviewV1InterpritationB ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    MockFunction<void( RequestFailTypeOpt )> callback;

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_THERMOSTAT_SETPOINT, 1 );
    auto cc = node->getCommandClass<ZCc0x43ThermostatSetpoint>( 0 );

    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

    uint8_t bitmask[32] = {
        0,
    };
    fillBitMap( bitmask, { 1, 2, 7, 8, 9, 10 } );
    znet_thermostat_setpoint_supported_report_t report{};
    report.bit_mask = bitmask;
    report.stcount = 32;
    node->getRequestsManager().onRespond( ThermostatSetpointSupportedRequest( 1, 0 ), report );

    EXPECT_FALSE( cc->hasInterviewDone() );
    node->getRequestsManager().onRespond(
        ThermostatAmbiguousSetpointRequest( 1, 0, 0x01 ), generateSimpleSetpointReport( 0x01 ), getIsRequested() );

    EXPECT_FALSE( cc->hasInterviewDone() );
    node->getRequestsManager().onRespond(
        ThermostatAmbiguousSetpointRequest( 1, 0, 0x02 ), generateSimpleSetpointReport( 0x02 ), getIsRequested() );

    EXPECT_FALSE( cc->hasInterviewDone() );
    node->getRequestsManager().onRespond(
        ThermostatAmbiguousSetpointRequest( 1, 0, 0x07 ), generateSimpleSetpointReport( 0x07 ), getIsRequested() );

    EXPECT_FALSE( cc->hasInterviewDone() );
    node->getRequestsManager().onRespond(
        ThermostatAmbiguousSetpointRequest( 1, 0, 0x08 ), generateSimpleSetpointReport( 0x08 ), getIsRequested() );

    EXPECT_FALSE( cc->hasInterviewDone() );
    node->getRequestsManager().onRespond(
        ThermostatAmbiguousSetpointRequest( 1, 0, 0x09 ), generateSimpleSetpointReport( 0x09 ), getIsRequested() );

    EXPECT_FALSE( cc->hasInterviewDone() );
    EXPECT_CALL( callback, Call( RequestFailTypeOpt() ) ).Times( 1 );
    node->getRequestsManager().onRespond(
        ThermostatAmbiguousSetpointRequest( 1, 0, 0x0A ), generateSimpleSetpointReport( 0x0A ), getIsRequested() );

    EXPECT_TRUE( cc->hasInterviewDone() );

    EXPECT_THAT( cc->getDescription(),
                 ElementsAre( generateSimpleDescription( 0x01 ),
                              generateSimpleDescription( 0x02 ),
                              generateSimpleDescription( 0x07 ),
                              generateSimpleDescription( 0x08 ),
                              generateSimpleDescription( 0x09 ),
                              generateSimpleDescription( 0x0A ) ) );
}

TEST_F( ZCcThermostatSetpointTestSuite, interviewV2InterpritationA ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    MockFunction<void( RequestFailTypeOpt )> callback;

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_THERMOSTAT_SETPOINT, 2 );
    auto cc = node->getCommandClass<ZCc0x43ThermostatSetpoint>( 0 );

    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

    uint8_t bitmask[32] = {
        0,
    };
    fillBitMap( bitmask, { 1, 2, 3, 4, 5, 6, 7, 8, 9 } );
    znet_thermostat_setpoint_supported_report_t report{};
    report.bit_mask = bitmask;
    report.stcount = 32;
    node->getRequestsManager().onRespond( ThermostatSetpointSupportedRequest( 1, 0 ), report );

    EXPECT_FALSE( cc->hasInterviewDone() );
    node->getRequestsManager().onRespond(
        ThermostatAmbiguousSetpointRequest( 1, 0, 0x01 ), generateSimpleSetpointReport( 0x01 ), getIsRequested() );

    EXPECT_FALSE( cc->hasInterviewDone() );
    node->getRequestsManager().onRespond(
        ThermostatAmbiguousSetpointRequest( 1, 0, 0x02 ), generateSimpleSetpointReport( 0x02 ), getIsRequested() );

    EXPECT_FALSE( cc->hasInterviewDone() );
    node->getRequestsManager().onRespond(
        ThermostatAmbiguousSetpointRequest( 1, 0, 0x07 ), generateSimpleSetpointReport( 0x07 ), getIsRequested() );

    EXPECT_FALSE( cc->hasInterviewDone() );
    node->getRequestsManager().onRespond(
        ThermostatAmbiguousSetpointRequest( 1, 0, 0x08 ), generateSimpleSetpointReport( 0x08 ), getIsRequested() );

    EXPECT_FALSE( cc->hasInterviewDone() );
    node->getRequestsManager().onRespond(
        ThermostatAmbiguousSetpointRequest( 1, 0, 0x09 ), generateSimpleSetpointReport( 0x09 ), getIsRequested() );

    EXPECT_FALSE( cc->hasInterviewDone() );
    node->getRequestsManager().onRespond(
        ThermostatAmbiguousSetpointRequest( 1, 0, 0x0A ), generateSimpleSetpointReport( 0x0A ), getIsRequested() );

    EXPECT_FALSE( cc->hasInterviewDone() );
    node->getRequestsManager().onRespond(
        ThermostatAmbiguousSetpointRequest( 1, 0, 0x0B ), generateSimpleSetpointReport( 0x0B ), getIsRequested() );

    EXPECT_FALSE( cc->hasInterviewDone() );
    node->getRequestsManager().onRespond(
        ThermostatAmbiguousSetpointRequest( 1, 0, 0x0C ), generateSimpleSetpointReport( 0x0C ), getIsRequested() );

    EXPECT_FALSE( cc->hasInterviewDone() );
    EXPECT_CALL( callback, Call( RequestFailTypeOpt() ) ).Times( 1 );
    node->getRequestsManager().onRespond(
        ThermostatAmbiguousSetpointRequest( 1, 0, 0x0D ), generateSimpleSetpointReport( 0x0D ), getIsRequested() );

    EXPECT_TRUE( cc->hasInterviewDone() );

    EXPECT_THAT( cc->getDescription(),
                 ElementsAre( generateSimpleDescription( 0x01 ),
                              generateSimpleDescription( 0x02 ),
                              generateSimpleDescription( 0x07 ),
                              generateSimpleDescription( 0x08 ),
                              generateSimpleDescription( 0x09 ),
                              generateSimpleDescription( 0x0A ),
                              generateSimpleDescription( 0x0B ),
                              generateSimpleDescription( 0x0C ),
                              generateSimpleDescription( 0x0D ) ) );
}

TEST_F( ZCcThermostatSetpointTestSuite, interviewV2InterpritationAAmbiguous ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    MockFunction<void( RequestFailTypeOpt )> callback;

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_THERMOSTAT_SETPOINT, 2 );
    auto cc = node->getCommandClass<ZCc0x43ThermostatSetpoint>( 0 );

    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

    uint8_t bitmask[32] = {
        0,
    };
    fillBitMap( bitmask, { 1, 2, 7, 8, 9 } );
    znet_thermostat_setpoint_supported_report_t report{};
    report.bit_mask = bitmask;
    report.stcount = 32;
    node->getRequestsManager().onRespond( ThermostatSetpointSupportedRequest( 1, 0 ), report );

    EXPECT_FALSE( cc->hasInterviewDone() );
    node->getRequestsManager().onRespond(
        ThermostatAmbiguousSetpointRequest( 1, 0, 0x01 ), generateSimpleSetpointReport( 0x01 ), getIsRequested() );

    EXPECT_FALSE( cc->hasInterviewDone() );
    node->getRequestsManager().onRespond(
        ThermostatAmbiguousSetpointRequest( 1, 0, 0x02 ), generateSimpleSetpointReport( 0x02 ), getIsRequested() );

    EXPECT_FALSE( cc->hasInterviewDone() );
    node->getRequestsManager().onRespond(
        ThermostatAmbiguousSetpointRequest( 1, 0, 0x00 ), generateUnsupportedSetpointReport(), getIsRequested() );

    EXPECT_FALSE( cc->hasInterviewDone() );
    node->getRequestsManager().onRespond(
        ThermostatAmbiguousSetpointRequest( 1, 0, 0x0B ), generateSimpleSetpointReport( 0x0B ), getIsRequested() );

    EXPECT_FALSE( cc->hasInterviewDone() );
    node->getRequestsManager().onRespond(
        ThermostatAmbiguousSetpointRequest( 1, 0, 0x00 ), generateUnsupportedSetpointReport(), getIsRequested() );

    EXPECT_FALSE( cc->hasInterviewDone() );
    node->getRequestsManager().onRespond(
        ThermostatAmbiguousSetpointRequest( 1, 0, 0x0C ), generateSimpleSetpointReport( 0x0C ), getIsRequested() );

    EXPECT_FALSE( cc->hasInterviewDone() );
    node->getRequestsManager().onRespond(
        ThermostatAmbiguousSetpointRequest( 1, 0, 0x00 ), generateUnsupportedSetpointReport(), getIsRequested() );

    EXPECT_FALSE( cc->hasInterviewDone() );
    EXPECT_CALL( callback, Call( RequestFailTypeOpt() ) ).Times( 1 );
    node->getRequestsManager().onRespond(
        ThermostatAmbiguousSetpointRequest( 1, 0, 0x0D ), generateSimpleSetpointReport( 0x0D ), getIsRequested() );

    EXPECT_TRUE( cc->hasInterviewDone() );

    EXPECT_THAT( cc->getDescription(),
                 ElementsAre( generateSimpleDescription( 0x01 ),
                              generateSimpleDescription( 0x02 ),
                              generateSimpleDescription( 0x0B ),
                              generateSimpleDescription( 0x0C ),
                              generateSimpleDescription( 0x0D ) ) );
}

TEST_F( ZCcThermostatSetpointTestSuite, interviewV2InterpritationB ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    MockFunction<void( RequestFailTypeOpt )> callback;

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_THERMOSTAT_SETPOINT, 2 );
    auto cc = node->getCommandClass<ZCc0x43ThermostatSetpoint>( 0 );

    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

    uint8_t bitmask[32] = {
        0,
    };
    fillBitMap( bitmask, { 1, 2, 7, 8, 9, 10, 11, 12, 13 } );
    znet_thermostat_setpoint_supported_report_t report{};
    report.bit_mask = bitmask;
    report.stcount = 32;
    node->getRequestsManager().onRespond( ThermostatSetpointSupportedRequest( 1, 0 ), report );

    EXPECT_FALSE( cc->hasInterviewDone() );
    node->getRequestsManager().onRespond(
        ThermostatAmbiguousSetpointRequest( 1, 0, 0x01 ), generateSimpleSetpointReport( 0x01 ), getIsRequested() );

    EXPECT_FALSE( cc->hasInterviewDone() );
    node->getRequestsManager().onRespond(
        ThermostatAmbiguousSetpointRequest( 1, 0, 0x02 ), generateSimpleSetpointReport( 0x02 ), getIsRequested() );

    EXPECT_FALSE( cc->hasInterviewDone() );
    node->getRequestsManager().onRespond(
        ThermostatAmbiguousSetpointRequest( 1, 0, 0x07 ), generateSimpleSetpointReport( 0x07 ), getIsRequested() );

    EXPECT_FALSE( cc->hasInterviewDone() );
    node->getRequestsManager().onRespond(
        ThermostatAmbiguousSetpointRequest( 1, 0, 0x0B ), generateSimpleSetpointReport( 0x0B ), getIsRequested() );

    EXPECT_FALSE( cc->hasInterviewDone() );
    node->getRequestsManager().onRespond(
        ThermostatAmbiguousSetpointRequest( 1, 0, 0x08 ), generateSimpleSetpointReport( 0x08 ), getIsRequested() );

    EXPECT_FALSE( cc->hasInterviewDone() );
    node->getRequestsManager().onRespond(
        ThermostatAmbiguousSetpointRequest( 1, 0, 0x0C ), generateSimpleSetpointReport( 0x0C ), getIsRequested() );

    EXPECT_FALSE( cc->hasInterviewDone() );
    node->getRequestsManager().onRespond(
        ThermostatAmbiguousSetpointRequest( 1, 0, 0x09 ), generateSimpleSetpointReport( 0x09 ), getIsRequested() );

    EXPECT_FALSE( cc->hasInterviewDone() );
    node->getRequestsManager().onRespond(
        ThermostatAmbiguousSetpointRequest( 1, 0, 0x0D ), generateSimpleSetpointReport( 0x0D ), getIsRequested() );

    EXPECT_FALSE( cc->hasInterviewDone() );
    EXPECT_CALL( callback, Call( RequestFailTypeOpt() ) ).Times( 1 );
    node->getRequestsManager().onRespond(
        ThermostatAmbiguousSetpointRequest( 1, 0, 0x0A ), generateSimpleSetpointReport( 0x0A ), getIsRequested() );

    EXPECT_TRUE( cc->hasInterviewDone() );

    EXPECT_THAT( cc->getDescription(),
                 ElementsAre( generateSimpleDescription( 0x01 ),
                              generateSimpleDescription( 0x02 ),
                              generateSimpleDescription( 0x07 ),
                              generateSimpleDescription( 0x0B ),
                              generateSimpleDescription( 0x08 ),
                              generateSimpleDescription( 0x0C ),
                              generateSimpleDescription( 0x09 ),
                              generateSimpleDescription( 0x0D ),
                              generateSimpleDescription( 0x0A ) ) );
}

TEST_F( ZCcThermostatSetpointTestSuite, interviewV2InterpritationBAmbiguous ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    MockFunction<void( RequestFailTypeOpt )> callback;

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_THERMOSTAT_SETPOINT, 2 );
    auto cc = node->getCommandClass<ZCc0x43ThermostatSetpoint>( 0 );

    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

    uint8_t bitmask[32] = {
        0,
    };
    fillBitMap( bitmask, { 1, 2, 7, 8, 9 } );
    znet_thermostat_setpoint_supported_report_t report{};
    report.bit_mask = bitmask;
    report.stcount = 32;
    node->getRequestsManager().onRespond( ThermostatSetpointSupportedRequest( 1, 0 ), report );

    EXPECT_FALSE( cc->hasInterviewDone() );
    node->getRequestsManager().onRespond(
        ThermostatAmbiguousSetpointRequest( 1, 0, 0x01 ), generateSimpleSetpointReport( 0x01 ), getIsRequested() );

    EXPECT_FALSE( cc->hasInterviewDone() );
    node->getRequestsManager().onRespond(
        ThermostatAmbiguousSetpointRequest( 1, 0, 0x02 ), generateSimpleSetpointReport( 0x02 ), getIsRequested() );

    EXPECT_FALSE( cc->hasInterviewDone() );
    node->getRequestsManager().onRespond(
        ThermostatAmbiguousSetpointRequest( 1, 0, 0x07 ), generateSimpleSetpointReport( 0x07 ), getIsRequested() );

    EXPECT_FALSE( cc->hasInterviewDone() );
    node->getRequestsManager().onRespond(
        ThermostatAmbiguousSetpointRequest( 1, 0, 0x00 ), generateUnsupportedSetpointReport(), getIsRequested() );

    EXPECT_FALSE( cc->hasInterviewDone() );
    node->getRequestsManager().onRespond(
        ThermostatAmbiguousSetpointRequest( 1, 0, 0x08 ), generateSimpleSetpointReport( 0x08 ), getIsRequested() );

    EXPECT_FALSE( cc->hasInterviewDone() );
    node->getRequestsManager().onRespond(
        ThermostatAmbiguousSetpointRequest( 1, 0, 0x00 ), generateUnsupportedSetpointReport(), getIsRequested() );

    EXPECT_FALSE( cc->hasInterviewDone() );
    node->getRequestsManager().onRespond(
        ThermostatAmbiguousSetpointRequest( 1, 0, 0x09 ), generateSimpleSetpointReport( 0x09 ), getIsRequested() );

    EXPECT_FALSE( cc->hasInterviewDone() );
    EXPECT_CALL( callback, Call( RequestFailTypeOpt() ) ).Times( 1 );
    node->getRequestsManager().onRespond(
        ThermostatAmbiguousSetpointRequest( 1, 0, 0x00 ), generateUnsupportedSetpointReport(), getIsRequested() );

    EXPECT_TRUE( cc->hasInterviewDone() );

    EXPECT_THAT( cc->getDescription(),
                 ElementsAre( generateSimpleDescription( 0x01 ),
                              generateSimpleDescription( 0x02 ),
                              generateSimpleDescription( 0x07 ),
                              generateSimpleDescription( 0x08 ),
                              generateSimpleDescription( 0x09 ) ) );
}

TEST_F( ZCcThermostatSetpointTestSuite, interviewV3 ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    MockFunction<void( RequestFailTypeOpt )> callback;

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_THERMOSTAT_SETPOINT, 3 );
    auto cc = node->getCommandClass<ZCc0x43ThermostatSetpoint>( 0 );

    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

    uint8_t bitmask[32] = {
        0,
    };
    fillBitMap( bitmask, { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 } );
    znet_thermostat_setpoint_supported_report_t report{};
    report.bit_mask = bitmask;
    report.stcount = 32;
    node->getRequestsManager().onRespond( ThermostatSetpointSupportedRequest( 1, 0 ), report );

    EXPECT_FALSE( cc->hasInterviewDone() );
    node->getRequestsManager().onRespond( ThermostatSetpointCapabilitiesRequest( 1, 0, 0x01 ),
                                          generateSimpleCapabilitiesReport( 0x01 ) );
    node->getRequestsManager().onRespond(
        ThermostatSetpointRequest( 1, 0, 0x01 ), generateSimpleSetpointReport( 0x01 ), getIsRequested() );

    EXPECT_FALSE( cc->hasInterviewDone() );
    node->getRequestsManager().onRespond( ThermostatSetpointCapabilitiesRequest( 1, 0, 0x02 ),
                                          generateSimpleCapabilitiesReport( 0x02 ) );
    node->getRequestsManager().onRespond(
        ThermostatSetpointRequest( 1, 0, 0x02 ), generateSimpleSetpointReport( 0x02 ), getIsRequested() );

    EXPECT_FALSE( cc->hasInterviewDone() );
    node->getRequestsManager().onRespond( ThermostatSetpointCapabilitiesRequest( 1, 0, 0x07 ),
                                          generateSimpleCapabilitiesReport( 0x07 ) );
    node->getRequestsManager().onRespond(
        ThermostatSetpointRequest( 1, 0, 0x07 ), generateSimpleSetpointReport( 0x07 ), getIsRequested() );

    EXPECT_FALSE( cc->hasInterviewDone() );
    node->getRequestsManager().onRespond( ThermostatSetpointCapabilitiesRequest( 1, 0, 0x08 ),
                                          generateSimpleCapabilitiesReport( 0x08 ) );
    node->getRequestsManager().onRespond(
        ThermostatSetpointRequest( 1, 0, 0x08 ), generateSimpleSetpointReport( 0x08 ), getIsRequested() );

    EXPECT_FALSE( cc->hasInterviewDone() );
    node->getRequestsManager().onRespond( ThermostatSetpointCapabilitiesRequest( 1, 0, 0x09 ),
                                          generateSimpleCapabilitiesReport( 0x09 ) );
    node->getRequestsManager().onRespond(
        ThermostatSetpointRequest( 1, 0, 0x09 ), generateSimpleSetpointReport( 0x09 ), getIsRequested() );

    EXPECT_FALSE( cc->hasInterviewDone() );
    node->getRequestsManager().onRespond( ThermostatSetpointCapabilitiesRequest( 1, 0, 0x0A ),
                                          generateSimpleCapabilitiesReport( 0x0A ) );
    node->getRequestsManager().onRespond(
        ThermostatSetpointRequest( 1, 0, 0x0A ), generateSimpleSetpointReport( 0x0A ), getIsRequested() );

    EXPECT_FALSE( cc->hasInterviewDone() );
    node->getRequestsManager().onRespond( ThermostatSetpointCapabilitiesRequest( 1, 0, 0x0B ),
                                          generateSimpleCapabilitiesReport( 0x0B ) );
    node->getRequestsManager().onRespond(
        ThermostatSetpointRequest( 1, 0, 0x0B ), generateSimpleSetpointReport( 0x0B ), getIsRequested() );

    EXPECT_FALSE( cc->hasInterviewDone() );
    node->getRequestsManager().onRespond( ThermostatSetpointCapabilitiesRequest( 1, 0, 0x0C ),
                                          generateSimpleCapabilitiesReport( 0x0C ) );
    node->getRequestsManager().onRespond(
        ThermostatSetpointRequest( 1, 0, 0x0C ), generateSimpleSetpointReport( 0x0C ), getIsRequested() );

    EXPECT_FALSE( cc->hasInterviewDone() );
    node->getRequestsManager().onRespond( ThermostatSetpointCapabilitiesRequest( 1, 0, 0x0D ),
                                          generateSimpleCapabilitiesReport( 0x0D ) );
    node->getRequestsManager().onRespond(
        ThermostatSetpointRequest( 1, 0, 0x0D ), generateSimpleSetpointReport( 0x0D ), getIsRequested() );

    EXPECT_FALSE( cc->hasInterviewDone() );
    node->getRequestsManager().onRespond( ThermostatSetpointCapabilitiesRequest( 1, 0, 0x0E ),
                                          generateSimpleCapabilitiesReport( 0x0E ) );
    node->getRequestsManager().onRespond(
        ThermostatSetpointRequest( 1, 0, 0x0E ), generateSimpleSetpointReport( 0x0E ), getIsRequested() );

    EXPECT_FALSE( cc->hasInterviewDone() );
    EXPECT_CALL( callback, Call( RequestFailTypeOpt() ) ).Times( 1 );
    node->getRequestsManager().onRespond( ThermostatSetpointCapabilitiesRequest( 1, 0, 0x0F ),
                                          generateSimpleCapabilitiesReport( 0x0F ) );
    node->getRequestsManager().onRespond(
        ThermostatSetpointRequest( 1, 0, 0x0F ), generateSimpleSetpointReport( 0x0F ), getIsRequested() );

    EXPECT_TRUE( cc->hasInterviewDone() );

    EXPECT_THAT( cc->getDescription(),
                 ElementsAre( generateSimpleDescription( 0x01 ),
                              generateSimpleDescription( 0x02 ),
                              generateSimpleDescription( 0x07 ),
                              generateSimpleDescription( 0x08 ),
                              generateSimpleDescription( 0x09 ),
                              generateSimpleDescription( 0x0A ),
                              generateSimpleDescription( 0x0B ),
                              generateSimpleDescription( 0x0C ),
                              generateSimpleDescription( 0x0D ),
                              generateSimpleDescription( 0x0E ),
                              generateSimpleDescription( 0x0F ) ) );
}

TEST_F( ZCcThermostatSetpointTestSuite, interviewV3Values ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    MockFunction<void( RequestFailTypeOpt )> callback;

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_THERMOSTAT_SETPOINT, 3 );
    auto cc = node->getCommandClass<ZCc0x43ThermostatSetpoint>( 0 );

    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

    uint8_t bitmask[32] = {
        0,
    };
    znet::utils::setBit( bitmask, 2 );
    znet::utils::setBit( bitmask, 5 );
    znet_thermostat_setpoint_supported_report_t report{};
    report.bit_mask = bitmask;
    report.stcount = 32;
    node->getRequestsManager().onRespond( ThermostatSetpointSupportedRequest( 1, 0 ), report );

    node->getRequestsManager().onRespond( ThermostatSetpointCapabilitiesRequest( 1, 0, 0x02 ), getCapabilities2() );

    znet_thermostat_setpoint_report_t report2{};
    report2.type = 2;
    report2.size = 4;
    report2.precision = 1;
    node->getRequestsManager().onRespond( ThermostatSetpointRequest( 1, 0, 2 ), report2, getIsRequested() );

    node->getRequestsManager().onRespond( ThermostatSetpointCapabilitiesRequest( 1, 0, 0x09 ), getCapabilities9() );

    EXPECT_FALSE( cc->hasInterviewDone() );
    EXPECT_CALL( callback, Call( RequestFailTypeOpt() ) ).Times( 1 );

    znet_thermostat_setpoint_report_t report9{};
    report9.type = 9;
    report9.size = 8;
    report9.precision = 3;
    node->getRequestsManager().onRespond( ThermostatSetpointRequest( 1, 0, 9 ), report9, getIsRequested() );

    EXPECT_TRUE( cc->hasInterviewDone() );

    ZCc0x43ThermostatSetpoint::setpointsDescription_t_v5 description2{};
    description2.type = 2;
    description2.size = 4;
    description2.precision = 1;
    description2.maxScale = 2;
    description2.minScale = 1;
    description2.minValue = 0.1f;
    description2.maxValue = 1.f;

    ZCc0x43ThermostatSetpoint::setpointsDescription_t_v5 description9{};
    description9.type = 9;
    description9.size = 8;
    description9.precision = 3;
    description9.maxScale = 3;
    description9.minScale = 0;
    description9.minValue = 0.2f;
    description9.maxValue = 2.f;

    EXPECT_THAT( cc->getDescription(), ElementsAre( description2, description9 ) );
}

TEST_F( ZCcThermostatSetpointTestSuite, requestValue ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_THERMOSTAT_SETPOINT, 3 );
    auto cc = node->getCommandClass<ZCc0x43ThermostatSetpoint>( 0 );
    cc->interview( {}, {} );

    uint8_t bitmask[32] = {
        0,
    };
    znet::utils::setBit( bitmask, 2 );
    znet::utils::setBit( bitmask, 5 );
    znet_thermostat_setpoint_supported_report_t report{};
    report.bit_mask = bitmask;
    report.stcount = 32;
    node->getRequestsManager().onRespond( ThermostatSetpointSupportedRequest( 1, 0 ), report );

    node->getRequestsManager().onRespond( ThermostatSetpointCapabilitiesRequest( 1, 0, 0x02 ), getCapabilities2() );

    znet_thermostat_setpoint_report_t report2{};
    report2.type = 2;
    report2.size = 4;
    report2.scale = 3;
    report2.sp_value = 1;
    report2.precision = 1;
    node->getRequestsManager().onRespond( ThermostatSetpointRequest( 1, 0, 0x02 ), report2, getIsRequested() );
    EXPECT_EQ( getIsRequested(), true );

    node->getRequestsManager().onRespond( ThermostatSetpointCapabilitiesRequest( 1, 0, 0x09 ), getCapabilities9() );

    znet_thermostat_setpoint_report_t report9{};
    report9.type = 5;
    report9.size = 8;
    report9.scale = 6;
    report9.sp_value = 2;
    report9.precision = 3;
    node->getRequestsManager().onRespond( ThermostatSetpointRequest( 1, 0, 0x09 ), report9, getIsRequested() );
    EXPECT_EQ( getIsRequested(), true );

    EXPECT_TRUE( cc->hasInterviewDone() );

    MockFunction<void( RequestFailTypeOpt )> callback;

    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_TRUE( cc->requestValue( createRequestValue( callback ) ) );

    node->getRequestsManager().onRespond( ThermostatSetpointRequest( 1, 0, 0x02 ), report2, getIsRequested() );
    EXPECT_EQ( getIsRequested(), true );

    EXPECT_CALL( callback, Call( RequestFailTypeOpt() ) ).Times( 1 );
    node->getRequestsManager().onRespond( ThermostatSetpointRequest( 1, 0, 0x09 ), report9, getIsRequested() );
    EXPECT_EQ( getIsRequested(), true );
}

}  // namespace ut
}  // namespace znet
