#include <znet/znet_node.hpp>
#include <znet/znet_command_class.hpp>

#include <znet/cc_0x33_color_switch.hpp>
#include <znet/cc_0x30_binary_sensor.hpp>
#include <znet/cc_0x31_sensor_multilevel.hpp>
#include <znet/cc_0x86_version.hpp>

#include <gtest/gtest.h>

#include "fake_file.h"

namespace znet {
namespace ut {

class ZNodeTestSuite : public ::testing::Test {
protected:
};

TEST_F( ZNodeTestSuite, addCommandClass ) {
    ZNode node( nullptr, nullptr, nullptr );

    {
        auto ccMultilevelSensor = node.addCommandClass( 0, ZNET_COMMAND_CLASS_SENSOR_MULTILEVEL );
        ASSERT_NE( ccMultilevelSensor, nullptr );

        EXPECT_EQ( ccMultilevelSensor->getId(), ZNET_COMMAND_CLASS_SENSOR_MULTILEVEL );
        EXPECT_EQ( ccMultilevelSensor->getVersion(), 1 );
        EXPECT_EQ( &ccMultilevelSensor->getParentNode(), &node );
        EXPECT_EQ( ccMultilevelSensor->getParentChannel().getId(), 0 );
    }

    {
        auto ccTime = node.addCommandClass( 0, ZNET_COMMAND_CLASS_TIME, 2 );
        ASSERT_NE( ccTime, nullptr );

        EXPECT_EQ( ccTime->getId(), ZNET_COMMAND_CLASS_TIME );
        EXPECT_EQ( ccTime->getVersion(), 2 );
        EXPECT_EQ( &ccTime->getParentNode(), &node );
        EXPECT_EQ( ccTime->getParentChannel().getId(), 0 );
    }

    {
        node.createAdditionalChannels( 1 );

        auto ccSwitch = node.addCommandClass( 1, ZNET_COMMAND_CLASS_SWITCH_BINARY );
        ASSERT_NE( ccSwitch, nullptr );

        EXPECT_EQ( ccSwitch->getId(), ZNET_COMMAND_CLASS_SWITCH_BINARY );
        EXPECT_EQ( ccSwitch->getVersion(), 1 );
        EXPECT_EQ( &ccSwitch->getParentNode(), &node );
        EXPECT_EQ( ccSwitch->getParentChannel().getId(), 1 );
    }

    {
        auto ccNotSupported = node.addCommandClass( 0, 0 );
        ASSERT_EQ( ccNotSupported, nullptr );
    }

    ASSERT_EQ( node.getCommandClassCount( 0 ), 2 );
    ASSERT_EQ( node.getCommandClassCount( 1 ), 1 );
}

TEST_F( ZNodeTestSuite, getCommandClass ) {
    ZNode node( nullptr, nullptr, nullptr );
    node.createAdditionalChannels( 1 );

    auto ccMultilevelSensor = node.addCommandClass( 0, ZNET_COMMAND_CLASS_SENSOR_MULTILEVEL );
    auto ccSensorBinary = node.addCommandClass( 0, ZNET_COMMAND_CLASS_SENSOR_BINARY );
    auto ccVersion = node.addCommandClass( 0, ZNET_COMMAND_CLASS_VERSION );
    auto ccSwitchColor = node.addCommandClass( 1, ZNET_COMMAND_CLASS_SWITCH_COLOR );

    ASSERT_NE( ccMultilevelSensor, nullptr );
    ASSERT_NE( ccSensorBinary, nullptr );
    ASSERT_NE( ccVersion, nullptr );
    ASSERT_NE( ccSwitchColor, nullptr );

    ASSERT_EQ( node.getCommandClass( 0, ZNET_COMMAND_CLASS_SENSOR_MULTILEVEL ), ccMultilevelSensor );
    ASSERT_EQ( node.getCommandClass( 0, ZNET_COMMAND_CLASS_SENSOR_BINARY ), ccSensorBinary );
    ASSERT_EQ( node.getCommandClass( 0, ZNET_COMMAND_CLASS_VERSION ), ccVersion );
    ASSERT_EQ( node.getCommandClass( 1, ZNET_COMMAND_CLASS_SWITCH_COLOR ), ccSwitchColor );

    ASSERT_EQ( node.getCommandClass<ZCc0x31SensorMultilevel>( 0 ), ccMultilevelSensor );
    ASSERT_EQ( node.getCommandClass<ZCc0x30BinarySensor>( 0 ), ccSensorBinary );
    ASSERT_EQ( node.getCommandClass<ZCc0x86Version>( 0 ), ccVersion );
    ASSERT_EQ( node.getCommandClass<ZCc0x33ColorSwitch>( 1 ), ccSwitchColor );

    ASSERT_EQ( node.getCommandClass( 1, ZNET_COMMAND_CLASS_SENSOR_MULTILEVEL ), nullptr );
    ASSERT_EQ( node.getCommandClass( 1, ZNET_COMMAND_CLASS_SENSOR_BINARY ), nullptr );
    ASSERT_EQ( node.getCommandClass( 1, ZNET_COMMAND_CLASS_VERSION ), nullptr );
    ASSERT_EQ( node.getCommandClass( 0, ZNET_COMMAND_CLASS_SWITCH_COLOR ), nullptr );
}

TEST_F( ZNodeTestSuite, getNextCommandClass ) {
    ZNode node( nullptr, nullptr, nullptr );
    node.createAdditionalChannels( 2 );

    auto ccMultilevelSensor0 = node.addCommandClass( 0, ZNET_COMMAND_CLASS_SENSOR_MULTILEVEL );
    auto ccMultilevelSensor1 = node.addCommandClass( 1, ZNET_COMMAND_CLASS_SENSOR_MULTILEVEL );
    auto ccSensorBinary0 = node.addCommandClass( 1, ZNET_COMMAND_CLASS_SENSOR_BINARY );

    auto ccFoundMultilevelSensor = node.getNextCommandClass<ZCc0x31SensorMultilevel>();
    auto ccFoundBinarySensor = node.getNextCommandClass<ZCc0x30BinarySensor>();
    auto ccFoundVersion = node.getNextCommandClass<ZCc0x86Version>();

    ASSERT_EQ( ccFoundMultilevelSensor, ccMultilevelSensor0 );
    ASSERT_EQ( ccFoundBinarySensor, ccSensorBinary0 );
    ASSERT_EQ( ccFoundVersion, nullptr );

    ccFoundMultilevelSensor = node.getNextCommandClass<ZCc0x31SensorMultilevel>( ccFoundMultilevelSensor );
    ccFoundBinarySensor = node.getNextCommandClass<ZCc0x30BinarySensor>( ccFoundBinarySensor );

    ASSERT_EQ( ccFoundMultilevelSensor, ccMultilevelSensor1 );
    ASSERT_EQ( ccFoundBinarySensor, nullptr );

    ccFoundMultilevelSensor = node.getNextCommandClass<ZCc0x31SensorMultilevel>( ccFoundMultilevelSensor );
    ASSERT_EQ( ccFoundMultilevelSensor, nullptr );
}

TEST_F( ZNodeTestSuite, storeRestore ) {
    FakeFile file;

    {
        ZNode node( nullptr, nullptr, nullptr );
        node.createAdditionalChannels( 1 );

        node.addCommandClass( 0, ZNET_COMMAND_CLASS_SENSOR_MULTILEVEL );
        node.addCommandClass( 0, ZNET_COMMAND_CLASS_ANTITHEFT, 3 );
        node.addCommandClass( 0, ZNET_COMMAND_CLASS_DOOR_LOCK, 4 );
        node.addCommandClass( 1, ZNET_COMMAND_CLASS_SENSOR_MULTILEVEL, 2 );
        node.addCommandClass( 1, ZNET_COMMAND_CLASS_SENSOR_BINARY, 2 );

        node.storeData( file );
    }

    {
        ZNode node( nullptr, nullptr, nullptr );
        node.restoreData( file );

        EXPECT_FALSE( node.isRediscoverRequired() );

        EXPECT_EQ( node.getChannelsCount(), 2 );
        EXPECT_EQ( node.getCommandClassCount( 0 ), 3 );
        EXPECT_EQ( node.getCommandClassCount( 1 ), 2 );

        ASSERT_NE( node.getCommandClass( 0, ZNET_COMMAND_CLASS_SENSOR_MULTILEVEL ), nullptr );
        EXPECT_EQ( node.getCommandClass( 0, ZNET_COMMAND_CLASS_SENSOR_MULTILEVEL )->getVersion(), 1 );

        ASSERT_NE( node.getCommandClass( 0, ZNET_COMMAND_CLASS_ANTITHEFT ), nullptr );
        EXPECT_EQ( node.getCommandClass( 0, ZNET_COMMAND_CLASS_ANTITHEFT )->getVersion(), 3 );

        ASSERT_NE( node.getCommandClass( 0, ZNET_COMMAND_CLASS_DOOR_LOCK ), nullptr );
        EXPECT_EQ( node.getCommandClass( 0, ZNET_COMMAND_CLASS_DOOR_LOCK )->getVersion(), 4 );

        ASSERT_NE( node.getCommandClass( 1, ZNET_COMMAND_CLASS_SENSOR_MULTILEVEL ), nullptr );
        EXPECT_EQ( node.getCommandClass( 1, ZNET_COMMAND_CLASS_SENSOR_MULTILEVEL )->getVersion(), 2 );

        ASSERT_NE( node.getCommandClass( 1, ZNET_COMMAND_CLASS_SENSOR_BINARY ), nullptr );
        EXPECT_EQ( node.getCommandClass( 1, ZNET_COMMAND_CLASS_SENSOR_BINARY )->getVersion(), 2 );
    }
}

TEST_F( ZNodeTestSuite, storeRestoreWithRequiredRediscover ) {
    FakeFile file;

    {
        ZNode node( nullptr, nullptr, nullptr );
        node.createAdditionalChannels( 1 );

        node.addCommandClass( 0, ZNET_COMMAND_CLASS_SENSOR_MULTILEVEL );
        node.addCommandClass( 0, ZNET_COMMAND_CLASS_ANTITHEFT, 3, false );
        node.addCommandClass( 0, ZNET_COMMAND_CLASS_DOOR_LOCK, 4 );
        node.addCommandClass( 1, ZNET_COMMAND_CLASS_SENSOR_MULTILEVEL, 2 );
        node.addCommandClass( 1, ZNET_COMMAND_CLASS_SENSOR_BINARY, 2 );

        node.storeData( file );
    }

    {
        ZNode node( nullptr, nullptr, nullptr );
        node.restoreData( file );

        EXPECT_TRUE( node.isRediscoverRequired() );

        EXPECT_EQ( node.getChannelsCount(), 2 );
        EXPECT_EQ( node.getCommandClassCount( 0 ), 2 );
        EXPECT_EQ( node.getCommandClassCount( 1 ), 2 );

        ASSERT_NE( node.getCommandClass( 0, ZNET_COMMAND_CLASS_SENSOR_MULTILEVEL ), nullptr );
        ASSERT_NE( node.getCommandClass( 0, ZNET_COMMAND_CLASS_DOOR_LOCK ), nullptr );
        ASSERT_NE( node.getCommandClass( 1, ZNET_COMMAND_CLASS_SENSOR_MULTILEVEL ), nullptr );
        ASSERT_NE( node.getCommandClass( 1, ZNET_COMMAND_CLASS_SENSOR_BINARY ), nullptr );
    }

    file = FakeFile();

    {
        ZNode node( nullptr, nullptr, nullptr );
        node.createAdditionalChannels( 1 );

        node.addCommandClass( 0, ZNET_COMMAND_CLASS_SENSOR_MULTILEVEL );
        node.addCommandClass( 0, ZNET_COMMAND_CLASS_SECURITY, 1, false );
        node.addCommandClass( 0, ZNET_COMMAND_CLASS_SECURITY_2, 1, false );
        node.addCommandClass( 1, ZNET_COMMAND_CLASS_SENSOR_MULTILEVEL, 2 );
        node.addCommandClass( 1, ZNET_COMMAND_CLASS_SENSOR_BINARY, 2 );

        node.storeData( file );
    }

    {
        ZNode node( nullptr, nullptr, nullptr );
        node.restoreData( file );

        EXPECT_FALSE( node.isRediscoverRequired() );

        EXPECT_EQ( node.getChannelsCount(), 2 );
        EXPECT_EQ( node.getCommandClassCount( 0 ), 1 );
        EXPECT_EQ( node.getCommandClassCount( 1 ), 2 );
    }

    file = FakeFile();

    {
        ZNode node( nullptr, nullptr, nullptr );
        node.createAdditionalChannels( 1 );

        node.addCommandClass( 0, ZNET_COMMAND_CLASS_SENSOR_MULTILEVEL );
        node.addCommandClass( 0, ZNET_COMMAND_CLASS_SECURITY, 1, false );
        node.addCommandClass( 0, ZNET_COMMAND_CLASS_SECURITY_2, 1, false );
        node.addCommandClass( 1, ZNET_COMMAND_CLASS_SENSOR_MULTILEVEL, 2 );
        node.addCommandClass( 1, ZNET_COMMAND_CLASS_SENSOR_BINARY, 2 );

        node.setRediscoverRequired( true );

        node.storeData( file );
    }

    {
        ZNode node( nullptr, nullptr, nullptr );
        node.restoreData( file );

        EXPECT_TRUE( node.isRediscoverRequired() );

        EXPECT_EQ( node.getChannelsCount(), 2 );
        EXPECT_EQ( node.getCommandClassCount( 0 ), 1 );
        EXPECT_EQ( node.getCommandClassCount( 1 ), 2 );
    }
}

}  // namespace ut
}  // namespace znet
