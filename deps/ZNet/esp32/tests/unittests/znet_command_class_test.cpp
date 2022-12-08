#include "fake_command_class_visitor.hpp"

#include <znet/znet_node.hpp>
#include <znet/znet_all_command_classes.hpp>

#include <gtest/gtest.h>

namespace znet {
namespace ut {

class ZClassTestSuite : public ::testing::Test {
protected:
};

TEST_F( ZClassTestSuite, visit ) {
    ZNode node( nullptr, nullptr, nullptr );

    auto ccMultilevelSensor = node.addCommandClass( 0, ZNET_COMMAND_CLASS_SENSOR_MULTILEVEL );
    auto ccSensorBinary = node.addCommandClass( 0, ZNET_COMMAND_CLASS_SENSOR_BINARY );
    auto ccVersion = node.addCommandClass( 0, ZNET_COMMAND_CLASS_VERSION );

    ASSERT_NE( ccMultilevelSensor, nullptr );
    ASSERT_NE( ccSensorBinary, nullptr );
    ASSERT_NE( ccVersion, nullptr );

    {
        auto visitor = visitors::makeOverloads( []( ZCc0x31SensorMultilevel& cc ) { cc.setVersion( 2 ); },
                                                []( ZCc0x30BinarySensor& cc ) { cc.setVersion( 3 ); },
                                                []( ZClass& cc ) { cc.setVersion( 4 ); } );

        visitors::visit( visitor, *ccMultilevelSensor );
        visitors::visit( visitor, *ccSensorBinary );
        visitors::visit( visitor, *ccVersion );

        EXPECT_EQ( ccMultilevelSensor->getVersion(), 2 );
        EXPECT_EQ( ccSensorBinary->getVersion(), 3 );
        EXPECT_EQ( ccVersion->getVersion(), 4 );
    }

    {
        auto visitor = visitors::makeOverloads(
            []( ZCc0x31SensorMultilevel& cc ) {
                cc.setVersion( 5 );
                return std::string( "multilevel_sensor" );
            },
            []( ZCc0x30BinarySensor& cc ) {
                cc.setVersion( 6 );
                return std::string( "binary_sensor" );
            },
            visitors::makeDefaultOverload( [] { return std::string( "default" ); } ) );

        EXPECT_EQ( visitors::visit( visitor, *ccMultilevelSensor ), "multilevel_sensor" );
        EXPECT_EQ( visitors::visit( visitor, *ccSensorBinary ), "binary_sensor" );
        EXPECT_EQ( visitors::visit( visitor, *ccVersion ), "default" );

        EXPECT_EQ( ccMultilevelSensor->getVersion(), 5 );
        EXPECT_EQ( ccSensorBinary->getVersion(), 6 );
        EXPECT_EQ( ccVersion->getVersion(), 4 );
    }

    {
        struct Vistor : FakeZClassVistor {
            void operator()( ZCc0x31SensorMultilevel& cc ) override {
                cc.setVersion( 7 );
            }
            void operator()( ZCc0x30BinarySensor& cc ) override {
                cc.setVersion( 8 );
            }
        };

        Vistor visitor;

        ccMultilevelSensor->accept( visitor );
        ccSensorBinary->accept( visitor );
        ccVersion->accept( visitor );

        EXPECT_EQ( ccMultilevelSensor->getVersion(), 7 );
        EXPECT_EQ( ccSensorBinary->getVersion(), 8 );
        EXPECT_EQ( ccVersion->getVersion(), 4 );

        visitors::visit( visitor, *ccMultilevelSensor );
        visitors::visit( visitor, *ccSensorBinary );
        visitors::visit( visitor, *ccVersion );
    }

    {
        struct Vistor : FakeZClassVistor {
            void operator()( ZCc0x31SensorMultilevel& cc ) override {
                cc.setVersion( 9 );
            }
            void operator()( ZCc0x30BinarySensor& cc ) override {
                cc.setVersion( 10 );
            }
        };

        Vistor visitor;

        visitors::visit( visitor, *ccMultilevelSensor );
        visitors::visit( visitor, *ccSensorBinary );
        visitors::visit( visitor, *ccVersion );

        EXPECT_EQ( ccMultilevelSensor->getVersion(), 9 );
        EXPECT_EQ( ccSensorBinary->getVersion(), 10 );
        EXPECT_EQ( ccVersion->getVersion(), 4 );
    }
}

}  // namespace ut
}  // namespace znet
