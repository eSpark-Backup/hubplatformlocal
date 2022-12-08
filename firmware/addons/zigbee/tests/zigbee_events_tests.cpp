#include "zigbee/script_events/attribute_value_updated.h"
#include "zigbee/script_events/include_device.h"
#include "zigbee/script_events/include_device_progress.h"
#include "zigbee/script_events/mfg_specific_attribute_value_updated.h"
#include "zigbee/script_events/module_reset.h"
#include "zigbee/script_events/node_added.h"
#include "zigbee/script_events/node_rejoined.h"
#include "zigbee/script_events/node_removed.h"
#include "zigbee/script_events/arm_command_received.h"
#include "zigbee/script_events/simple_command_received.h"
#include "zigbee/script_events/reachable_state_updated.h"
#include "zigbee/script_events/get_user_response_command_received.h"
#include "zigbee/script_events/door_lock_operation_event_notification_received.h"
#include "zigbee/script_events/door_lock_programming_event_notification_received.h"
#include "zigbee/script_events/user_codes_request_progress.h"
#include "zigbee/script_events/alert_received.h"
#include "zigbee/script_events/empty_alerts_list_response_received.h"

#include "zigbee/resources/zigbee_resources.h"
#include "zigbee/resources/enums/mode_status.h"

#include "zigbee/model/zigbee_attribute.h"
#include "zigbee/model/enums/commands/user_type.h"
#include "zigbee/model/enums/commands/user_status.h"
#include "zigbee/model/enums/commands/door_lock_event_source.h"
#include "zigbee/model/enums/commands/door_lock_operation_event.h"
#include "zigbee/model/enums/commands/door_lock_programming_event.h"
#include "zigbee/model/enums/commands/alert_notification_cotegory_source.h"
#include "zigbee/model/enums/commands/get_alerts_responce_source.h"

#include "json_utils/json_utils.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

namespace HubPlatform {
namespace Zigbee {
namespace UT {
using namespace Resources::ScriptEvent;
using namespace ::testing;

class ZigbeeEventToJSON : public Test {
public:
    std::unique_ptr< ZigbeeAttribute > attribute() {
        return ZigbeeAttributeFactory::createZigbeeAttribute( 0x0006, 0x0000, AttributeValueType::BooleanData, false );
    }

    Json renameIdField( Json&& obj ) {
        obj[ "id" ] = obj[ "_id" ];
        obj.erase( "_id" );
        return obj;
    }
};

TEST_F( ZigbeeEventToJSON, moduleResetEvent ) {
    ModuleReset event{ ModeStatus::Started };
    const auto joEvent = event.toJson();
    EXPECT_EQ( joEvent.size(), 2 );
    EXPECT_EQ( joEvent[ "event" ], Name::ModuleReset );
    EXPECT_EQ( joEvent[ Param::Status ], Param::ActionStatus::Started );
}

TEST_F( ZigbeeEventToJSON, includeDeviceEvent ) {
    IncludeDevice event{ ModeStatus::Started };
    const auto joEvent = event.toJson();
    EXPECT_EQ( joEvent.size(), 2 );
    EXPECT_EQ( joEvent[ "event" ], Name::IncludeDevice );
    EXPECT_EQ( joEvent[ Param::Status ], Param::ActionStatus::Started );
}

TEST_F( ZigbeeEventToJSON, nodeAddedEvent ) {
    const auto nodeId = 5;

    NodeAdded event( nodeId );
    const auto joEvent = event.toJson();
    EXPECT_EQ( joEvent.size(), 2 );
    EXPECT_EQ( joEvent[ "event" ], Name::NodeAdded );
    EXPECT_EQ( joEvent[ Param::NodeId ], nodeId );
}

TEST_F( ZigbeeEventToJSON, nodeRejoinedEvent ) {
    const auto nodeId = 5;

    NodeRejoined event( nodeId );
    const auto joEvent = event.toJson();
    EXPECT_EQ( joEvent.size(), 2 );
    EXPECT_EQ( joEvent[ "event" ], Name::NodeRejoined );
    EXPECT_EQ( joEvent[ Param::NodeId ], nodeId );
}

TEST_F( ZigbeeEventToJSON, nodeRemovedEvent ) {
    const auto nodeId = 5;
    const auto manufacturerId = "man_test";
    const auto modelId = "model_test";

    NodeRemoved event( nodeId, manufacturerId, modelId );
    const auto joEvent = event.toJson();
    EXPECT_EQ( joEvent.size(), 4 );
    EXPECT_EQ( joEvent[ "event" ], Name::NodeRemoved );
    EXPECT_EQ( joEvent[ Param::NodeId ], nodeId );
    EXPECT_EQ( joEvent[ Param::ManufacturerId ], manufacturerId );
    EXPECT_EQ( joEvent[ Param::ModelId ], modelId );
}

TEST_F( ZigbeeEventToJSON, attributeValueUpdatedEvent ) {
    const auto nodeId = 5;
    const auto endpointId = 1;
    const auto clusterName = "on_off";
    auto attribute = this->attribute();

    AttributeValueUpdated event( nodeId, endpointId, clusterName, *attribute );
    const auto joEvent = event.toJson();
    EXPECT_EQ( joEvent.size(), 5 );
    EXPECT_EQ( joEvent[ "event" ], Name::AttributeValueUpdated );
    EXPECT_EQ( joEvent[ Param::NodeId ], nodeId );
    EXPECT_EQ( joEvent[ Param::EndpointId ], endpointId );
    EXPECT_EQ( joEvent[ Param::ClusterName ], clusterName );
    EXPECT_EQ( joEvent[ Param::Attribute ].dump(), renameIdField( attribute->toJson() ).dump() );
}

TEST_F( ZigbeeEventToJSON, mfgSpecificAttributeValueUpdatedEvent ) {
    const auto nodeId = 5;
    const auto endpointId = 1;
    const auto clusterId = 0x0006;
    auto attribute = this->attribute();

    MfgSpecificAttributeValueUpdated event( nodeId, endpointId, clusterId, *attribute );
    const auto joEvent = event.toJson();
    EXPECT_EQ( joEvent.size(), 5 );
    EXPECT_EQ( joEvent[ "event" ], Name::MfgSpecificAttributeValueUpdated );
    EXPECT_EQ( joEvent[ Param::NodeId ], nodeId );
    EXPECT_EQ( joEvent[ Param::EndpointId ], endpointId );
    EXPECT_EQ( joEvent[ Param::ClusterId ], clusterId );
    EXPECT_EQ( joEvent[ Param::Attribute ].dump(), renameIdField( attribute->toJson() ).dump() );
}

TEST_F( ZigbeeEventToJSON, armCommandReceivedEvent ) {
    const auto nodeId = 5;
    const auto endpointId = 1;
    const auto pinCode = "1111";
    const auto mode = IasAceArmCommandMode::Disarm;

    ArmCommandReceived event( nodeId, endpointId, mode, pinCode );
    const auto joEvent = event.toJson();
    EXPECT_EQ( joEvent.size(), 8 );
    EXPECT_EQ( joEvent[ "event" ], Name::ArmCommandReceived );
    EXPECT_EQ( joEvent[ Param::NodeId ], nodeId );
    EXPECT_EQ( joEvent[ Param::EndpointId ], endpointId );
    EXPECT_EQ( joEvent[ Param::ClusterName ], "ias_ace" );
    EXPECT_EQ( joEvent[ Param::CommandId ], 0x00 );
    EXPECT_EQ( joEvent[ Param::CommandName ], "Arm" );
    EXPECT_EQ( joEvent[ Param::ArmMode ], "disarm" );
    EXPECT_EQ( joEvent[ Param::PinCode ], pinCode );
}

TEST_F( ZigbeeEventToJSON, getUserResponseCommandReceivedEvent ) {
    const auto nodeId = 5;
    const auto endpointId = 1;
    const auto userId = 2;
    UserStatus userStatus = UserStatus::OccupiedEnabled;
    UserType userType = UserType::UnrestrictedUser;
    const auto pinCode = "1111";

    GetUserResponseReceived event( nodeId, endpointId, userId, userStatus, userType, pinCode );
    const auto joEvent = event.toJson();
    EXPECT_EQ( joEvent.size(), 10 );
    EXPECT_EQ( joEvent[ "event" ], Name::GetUserResponseCommandReceived );
    EXPECT_EQ( joEvent[ Param::NodeId ], nodeId );
    EXPECT_EQ( joEvent[ Param::EndpointId ], endpointId );
    EXPECT_EQ( joEvent[ Param::ClusterName ], "door_lock" );
    EXPECT_EQ( joEvent[ Param::CommandId ], 0x06 );
    EXPECT_EQ( joEvent[ Param::CommandName ], "GetPinCodeResponse" );
    EXPECT_EQ( joEvent[ Param::UserId ], userId );
    EXPECT_EQ( joEvent[ Param::UserStatus ], "occupied_enabled" );
    EXPECT_EQ( joEvent[ Param::UserType ], "unrestricted_user" );
    EXPECT_EQ( joEvent[ Param::PinCode ], pinCode );
}

TEST_F( ZigbeeEventToJSON, doorLockOperatingEventNotificationReceivedEvent ) {
    const auto nodeId = 5;
    const auto endpointId = 1;
    DoorLockEventSource eventSource = DoorLockEventSource::Keypad;
    DoorLockOperationEvent eventType = DoorLockOperationEvent::KeyLock;
    const auto userId = 2;
    const auto pinCode = "1111";

    DoorLockOperatingEventNotificationReceived event( nodeId, endpointId, eventSource, eventType, userId, pinCode );
    const auto joEvent = event.toJson();
    EXPECT_EQ( joEvent.size(), 10 );
    EXPECT_EQ( joEvent[ "event" ], Name::DoorLockOperatingEventNotificationReceived );
    EXPECT_EQ( joEvent[ Param::NodeId ], nodeId );
    EXPECT_EQ( joEvent[ Param::EndpointId ], endpointId );
    EXPECT_EQ( joEvent[ Param::ClusterName ], "door_lock" );
    EXPECT_EQ( joEvent[ Param::CommandId ], 0x20 );
    EXPECT_EQ( joEvent[ Param::CommandName ], "OperatingEventNotification" );
    EXPECT_EQ( joEvent[ Param::UserId ], userId );
    EXPECT_EQ( joEvent[ Param::EventSource ], "keypad" );
    EXPECT_EQ( joEvent[ Param::EventName ], "key_lock" );
    EXPECT_EQ( joEvent[ Param::PinCode ], pinCode );
}

TEST_F( ZigbeeEventToJSON, doorLockProgrammingEventNotificationReceivedEvent ) {
    const auto nodeId = 5;
    const auto endpointId = 1;
    DoorLockEventSource eventSource = DoorLockEventSource::Keypad;
    DoorLockProgrammingEvent eventType = DoorLockProgrammingEvent::PINCodeAdded;
    const auto userId = 2;
    const auto pinCode = "1111";
    UserStatus userStatus = UserStatus::OccupiedEnabled;
    UserType userType = UserType::UnrestrictedUser;

    DoorLockProgrammingEventNotificationReceived event(
        nodeId, endpointId, eventSource, eventType, userId, pinCode, userType, userStatus );
    const auto joEvent = event.toJson();
    EXPECT_EQ( joEvent.size(), 12 );
    EXPECT_EQ( joEvent[ "event" ], Name::DoorLockProgrammingEventNotificationReceived );
    EXPECT_EQ( joEvent[ Param::NodeId ], nodeId );
    EXPECT_EQ( joEvent[ Param::EndpointId ], endpointId );
    EXPECT_EQ( joEvent[ Param::ClusterName ], "door_lock" );
    EXPECT_EQ( joEvent[ Param::CommandId ], 0x21 );
    EXPECT_EQ( joEvent[ Param::CommandName ], "ProgrammingEventNotification" );
    EXPECT_EQ( joEvent[ Param::UserId ], userId );
    EXPECT_EQ( joEvent[ Param::EventSource ], "keypad" );
    EXPECT_EQ( joEvent[ Param::EventName ], "pin_code_added" );
    EXPECT_EQ( joEvent[ Param::PinCode ], pinCode );
    EXPECT_EQ( joEvent[ Param::UserStatus ], "occupied_enabled" );
    EXPECT_EQ( joEvent[ Param::UserType ], "unrestricted_user" );
}

TEST_F( ZigbeeEventToJSON, simpleCommandReceivedEvent ) {
    const auto nodeId = 5;
    const auto endpointId = 1;
    const auto clusterName = "ias_ace";
    const auto commandId = 0x00;
    const auto commandName = "Panic";

    SimpleCommandReceived event( nodeId, endpointId, clusterName, commandId, commandName );
    const auto joEvent = event.toJson();
    EXPECT_EQ( joEvent.size(), 6 );
    EXPECT_EQ( joEvent[ "event" ], Name::SimpleCommandReceived );
    EXPECT_EQ( joEvent[ Param::NodeId ], nodeId );
    EXPECT_EQ( joEvent[ Param::EndpointId ], endpointId );
    EXPECT_EQ( joEvent[ Param::ClusterName ], clusterName );
    EXPECT_EQ( joEvent[ Param::CommandId ], commandId );
    EXPECT_EQ( joEvent[ Param::CommandName ], commandName );
}

TEST_F( ZigbeeEventToJSON, reachableStateUpdatedEvent ) {
    const auto nodeId = 5;
    const auto isReachable = true;

    ReachableStateUpdated event( nodeId, isReachable );
    const auto joEvent = event.toJson();
    EXPECT_EQ( joEvent.size(), 3 );
    EXPECT_EQ( joEvent[ "event" ], Name::ReachableStateUpdated );
    EXPECT_EQ( joEvent[ Param::NodeId ], nodeId );
    EXPECT_EQ( joEvent[ Param::Value ], isReachable );
}

TEST_F( ZigbeeEventToJSON, includeDeviceProgressEvent ) {
    const auto progress = 50;

    IncludeDeviceProgress event( progress );
    const auto joEvent = event.toJson();
    EXPECT_EQ( joEvent.size(), 2 );
    EXPECT_EQ( joEvent[ "event" ], Name::IncludeDeviceProgress );
    EXPECT_EQ( joEvent[ Param::Value ], progress );
}

TEST_F( ZigbeeEventToJSON, userCodeRequestProgressEvent ) {
    const auto nodeId = 42;
    const auto endpointId = 1;
    const auto progress = 50;

    UserCodesRequestProgress event( nodeId, endpointId, progress );
    const auto joEvent = event.toJson();
    EXPECT_EQ( joEvent.size(), 4 );
    EXPECT_EQ( joEvent[ "event" ], Name::UserCodesRequestProgress );
    EXPECT_EQ( joEvent[ Param::NodeId ], nodeId );
    EXPECT_EQ( joEvent[ Param::EndpointId ], endpointId );
    EXPECT_EQ( joEvent[ Param::Value ], progress );
}

TEST_F( ZigbeeEventToJSON, alertsNotificationReceived ) {
    const auto nodeId = 5;
    const auto endpointId = 1;
    const AlertStructure alertStructure = { 129,
                                            AlertNotificationCategory::Category::Warning,
                                            AlertNotificationPresenceRecovery::PresenceRecovery::Presence,
                                            0 };
    const char* commandName = "AlertsNotification";
    const AlertSource eventNotificationId = AlertSource::Notification;

    AlertReceived event( nodeId, endpointId, alertStructure, commandName, eventNotificationId );
    const auto joEvent = event.toJson();

    EXPECT_EQ( joEvent.size(), 10 );
    EXPECT_EQ( joEvent[ "event" ], Name::AlertReceived );
    EXPECT_EQ( joEvent[ Param::NodeId ], nodeId );
    EXPECT_EQ( joEvent[ Param::EndpointId ], endpointId );
    EXPECT_EQ( joEvent[ Param::AlertId ], alertStructure.alert_id );
    EXPECT_EQ( joEvent[ Param::Category ], AlertNotificationCategory::toString( alertStructure.category ) );
    EXPECT_EQ( joEvent[ Param::PresenceRecovery ],
               AlertNotificationPresenceRecovery::toString( alertStructure.presence_recovery ) );
    EXPECT_EQ( joEvent[ Param::ManufacturerSpecificBits ], alertStructure.manufacturer_specific_bits );
    EXPECT_EQ( joEvent[ Param::CommandName ], commandName );
    EXPECT_EQ( joEvent[ Param::CommandId ], 0x01 );
}

TEST_F( ZigbeeEventToJSON, emptyAlertsListResponseReceived ) {
    const auto nodeId = 5;
    const auto endpointId = 1;

    EmptyAlertsListResponseReceived event( nodeId, endpointId );
    const auto joEvent = event.toJson();

    EXPECT_EQ( joEvent.size(), 6 );
    EXPECT_EQ( joEvent[ "event" ], Name::EmptyAlertsListReceived );
    EXPECT_EQ( joEvent[ Param::NodeId ], nodeId );
    EXPECT_EQ( joEvent[ Param::EndpointId ], endpointId );
    EXPECT_EQ( joEvent[ Param::CommandName ],
               Resources::Fields::Command::Names::ApplianceEventsAndAlerts::GetAlertsResponse );
    EXPECT_EQ( joEvent[ Param::CommandId ], 0x00 );
}

}  // namespace UT
}  // namespace Zigbee
}  // namespace HubPlatform
