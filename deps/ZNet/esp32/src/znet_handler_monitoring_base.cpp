#include <znet/znet.h>
#include <cmath>
#include <znet_handler_monitoring_base.hpp>
#include <znet_impl.hpp>
#include <znet/iznet.hpp>
#include <znet/znet_node.hpp>
#include <znet/znet_utils.hpp>
#include <znet/cc_0x26_switch_multi_level.hpp>
#include <znet/cc_0x31_sensor_multilevel.hpp>
#include <znet/cc_0x32_meter.hpp>
#include <znet/cc_0x33_color_switch.hpp>
#include <znet/cc_0x43_thermostat_setpoint.hpp>
#include <znet/cc_0x40_thermostat_mode.hpp>
#include <znet/cc_0x42_thermostat_operating_state.hpp>
#include <znet/cc_0x45_thermostat_fan_state.hpp>
#include <znet/cc_0x63_user_code.hpp>
#include <znet/cc_0x66_barrier_operator.hpp>
#include <znet/cc_0x80_battery.hpp>
#include <znet/cc_0x84_wakeup.hpp>
#include <znet/cc_0x85_association.hpp>
#include <znet/cc_0x8e_multi_channel_association.hpp>
#include <znet/cc_0x62_door_lock.hpp>
#include <znet/cc_0x4e_schedule_entry_lock.hpp>
#include <znet/cc_0x7a_firmware_update.hpp>

#include <znet/converters/barrier_operator_state_converter.hpp>
#include <znet/converters/central_scene_attributes_converter.hpp>

extern "C" {
#include <znet_log.h>
}

#include <cassert>

namespace znet {

bool MonitoringBaseHandler::onLoop() {
    m_parentMesh.checkReachableNodesState();
    return true;
}

void MonitoringBaseHandler::onNodeCmdBasic( const uint8_t nodeId,
                                            const uint8_t channelId,
                                            const uint8_t currentValue,
                                            uint8_t targetValue ) {
    ZNode* node = m_parentMesh.onFindNode( nodeId );
    if( node == nullptr ) {
        ZNET_LOGE( "%s could not find node info  Id = %d\n", __func__, nodeId );
        return;
    }

    updateReachableState( *node );

    IZNetOwner& owner = m_parentMesh.getOwner();
    owner.onUpdateValue( nodeId,
                         ZNET_COMMAND_CLASS_BASIC,
                         channelId,
                         currentValue,
                         targetValue != ZNET_CMD_BASIC_VALUE_UNKNOWN ? Uint8Opt( targetValue ) : Uint8Opt(),
                         {} );
}

void MonitoringBaseHandler::onNodeCmdBasicSet( const uint8_t nodeId,
                                               const uint8_t channelId,
                                               const uint8_t currentValue,
                                               uint8_t targetValue ) {
    onNodeCmdBasic( nodeId, channelId, currentValue, targetValue );
}

void MonitoringBaseHandler::onForcedRequestResult() {
    ZNET_LOGI( "%s timer event\n", __func__ );  // for check, finally will be removed
    int destroy = -1;
    for( uint8_t nodeId = ZNET_NODE_ID_MIN; nodeId <= ZNET_NODE_ID_MAX; nodeId++ ) {
        ZNode* node = m_parentMesh.onFindNode( nodeId );
        if( node != nullptr && node->hasForcedRequests() ) {
            ZNode::ClassPtr ccTemp = node->getCommandClass( ZNET_CHANNEL_ID_ROOT, ZNET_COMMAND_CLASS_BATTERY );
            if( ccTemp ) {
                ZCc0x80Battery* cc = static_cast<ZCc0x80Battery*>( ccTemp.get() );
                if( cc != nullptr && cc->getId() == ZNET_COMMAND_CLASS_BATTERY && cc->hasForcedRequest() ) {
                    RequestValue request;
                    cc->requestValue( request );
                    destroy = 0;
                }
            }
        }
    }
    znet_forced_request_timer_create( destroy );
}

void MonitoringBaseHandler::onNodeRem(::znet_error_t err, const ::znet_nodeinfo_t* nodeInfo ) {
    if( err != ZNET_ERR_OK || !nodeInfo ) {
        ZNET_LOGE( "%s fail\n", __func__ );
        m_parentMesh.setState(::znet::IZNet::Mode::ExcludeStarting,
                              ZNET_ERR_TIME == err ? ::znet::ModeStatus::Timeout : ::znet::ModeStatus::Failed,
                              err != ZNET_ERR_OK ? err::message( err ) : err::message( ZNET_ERR_NIF_INVAL ) );
        if( nodeInfo ) {
            if( !m_parentMesh.sendNop( nodeInfo->node_id ) ) {
                m_parentMesh.activateMode(::znet::IZNet::Mode::Monitoring );
            }
        }
        else {
            m_parentMesh.activateMode(::znet::IZNet::Mode::Monitoring );
        }
    }
    else {
        ZNET_LOGI( "%s (node id: %hhu)\n", __func__, nodeInfo->node_id );
        m_parentMesh.setState(::znet::IZNet::Mode::ExcludeStarting, ::znet::ModeStatus::Started );
        m_parentMesh.getOwner().nodeRemove( nodeInfo->node_id );
        m_parentMesh.resetDeviceSecurityData( nodeInfo->node_id,
                                              ::znet::ProvisioningListEntryFields::Status::NOT_DETECTED );
        m_parentMesh.setState(::znet::IZNet::Mode::ExcludeStarting, ::znet::ModeStatus::Finished );
        m_parentMesh.activateMode(::znet::IZNet::Mode::Monitoring );
    }
}

void MonitoringBaseHandler::onNodeCmdBatteryLevel( const uint8_t nodeId,
                                                   const uint8_t channelId,
                                                   const uint8_t value ) {
    ZNode* node = m_parentMesh.onFindNode( nodeId );
    if( node == nullptr ) {
        ZNET_LOGE( "%s could not find node info  Id = %d\n", __func__, nodeId );
        return;
    }
    else {
        ZNET_LOGI( "%s battery level(%d) for node Id = %d\n", __func__, value, nodeId );
    }

    updateReachableState( *node );

    IZNetOwner& owner = m_parentMesh.getOwner();
    owner.onUpdateValue( nodeId, ZNET_COMMAND_CLASS_BATTERY, channelId, value );
}

void MonitoringBaseHandler::onNodeCmdBinarySwitch( const uint8_t nodeId,
                                                   const uint8_t channelId,
                                                   const uint8_t value,
                                                   const Uint8Opt targetValue,
                                                   const SecondsOpt duration ) {
    ZNode* node = m_parentMesh.onFindNode( nodeId );
    if( node == nullptr ) {
        ZNET_LOGE( "%s could not find node info  Id = %d\n", __func__, nodeId );
        return;
    }

    updateReachableState( *node );

    IZNetOwner& owner = m_parentMesh.getOwner();
    owner.onUpdateValue( nodeId, ZNET_COMMAND_CLASS_SWITCH_BINARY, channelId, value, targetValue, duration );
}

void MonitoringBaseHandler::onNodeCmdColorSwitch( const uint8_t nodeId,
                                                  const uint8_t channelId,
                                                  const ColorComponentID componentId,
                                                  const uint8_t value,
                                                  const utils::optional<uint8_t> targetValue,
                                                  const SecondsOpt targetDuration ) {
    ZNode* node = m_parentMesh.onFindNode( nodeId );
    if( node == nullptr ) {
        ZNET_LOGE( "%s could not find node info  Id = %d\n", __func__, nodeId );
        return;
    }
    updateReachableState( *node );

    IZNetOwner& owner = m_parentMesh.getOwner();
    auto cc = node->getCommandClass<ZCc0x33ColorSwitch>( channelId );
    if( cc ) {
        cc->getUpdatedComponentsCache().addComponent( { componentId, value, targetValue, targetDuration } );

        if( cc->getUpdatedComponentsCache().isCompleted() ) {
            owner.onUpdateRgbValue(
                nodeId, ZNET_COMMAND_CLASS_SWITCH_COLOR, channelId, cc->getUpdatedComponentsCache().takeComponents() );
        }
    }
}

void MonitoringBaseHandler::onNodeCmdDoorLockOperation( const uint8_t nodeId,
                                                        const uint8_t channelId,
                                                        const DoorLock::Mode mode,
                                                        const uint8_t insideHandleModes,
                                                        const uint8_t outsideHandleModes,
                                                        const uint8_t condition,
                                                        SecondsOpt lockTimeout,
                                                        const DoorLock::ModeOpt tagretMode,
                                                        const SecondsOpt tagretModeTimeOut ) {
    ZNode* node = m_parentMesh.onFindNode( nodeId );
    if( node == nullptr ) {
        ZNET_LOGE( "%s could not find node info  Id = %d\n", __func__, nodeId );
        return;
    }

    updateReachableState( *node );

    IZNetOwner& owner = m_parentMesh.getOwner();
    owner.onUpdateValueDoorLockOperation( nodeId,
                                          channelId,
                                          mode,
                                          insideHandleModes,
                                          outsideHandleModes,
                                          ZCc0x62DoorLock::decodeDoorCondition( condition ),
                                          lockTimeout,
                                          tagretMode,
                                          tagretModeTimeOut );
}

void MonitoringBaseHandler::onNodeCmdDoorLockConfiguration(
    const uint8_t nodeId,
    const uint8_t channelId,
    const znet_door_lock_configuration_t& configuration,
    const znet_door_lock_extended_configuration_t* extendedConfiguration ) {
    ZNode* node = m_parentMesh.onFindNode( nodeId );
    if( node == nullptr ) {
        ZNET_LOGE( "%s could not find node info  Id = %d\n", __func__, nodeId );
        return;
    }

    updateReachableState( *node );

    IZNetOwner& owner = m_parentMesh.getOwner();
    owner.onUpdateValueDoorLockConfiguration( nodeId, channelId, configuration, extendedConfiguration );
}

void MonitoringBaseHandler::onNodeCmdNotification( const uint8_t nodeId,
                                                   const uint8_t channelId,
                                                   const znet_notification_report_t& value,
                                                   bool isNotificationRequested ) {
    ZNode* node = m_parentMesh.onFindNode( nodeId );
    if( node == nullptr ) {
        ZNET_LOGE( "%s could not find node info  Id = %d\n", __func__, nodeId );
        return;
    }

    if( node->isRediscoverRequired() ) {
        m_parentMesh.interviewNode( nodeId );
        return;
    }

    updateReachableState( *node );

    const auto notificationType = static_cast<NotificationType>( value.notification_type );
    const NotificationEvent notificationEvent( value.notification_event );

    auto& owner = m_parentMesh.getOwner();
    if( !getNotificationName( notificationType ) ) {
        ZNET_LOGE( "%s unknown notification = %d\n", __func__, value.notification_type );
        owner.onUnknownNotification( nodeId,
                                     channelId,
                                     value.v1_alarm_type,
                                     value.v1_alarm_level,
                                     notificationType,
                                     notificationEvent,
                                     value.event_parameter,
                                     value.event_parameters_length );
        return;
    }

    auto cc = node->getCommandClass( channelId, ZNET_COMMAND_CLASS_NOTIFICATION );
    if( !cc ) {
        ZNET_LOGE( "%s could not find command class info  Id = 0x%x\n", __func__, ZNET_COMMAND_CLASS_NOTIFICATION );
        return;
    }

    const auto notificationEventData = getNotificationEventData( notificationType, notificationEvent );
    bool isWokenup = node->isWokenUp();

    const auto isKnownNotification =
        notificationEventData && cc && cc->getVersion() >= notificationEventData->m_version;

    if( isKnownNotification ) {
        owner.onNotification( nodeId,
                              channelId,
                              value.v1_alarm_type,
                              value.v1_alarm_level,
                              notificationType,
                              notificationEvent,
                              value.event_parameter,
                              value.event_parameters_length );
    }
    else {
        owner.onUnknownNotification( nodeId,
                                     channelId,
                                     value.v1_alarm_type,
                                     value.v1_alarm_level,
                                     notificationType,
                                     notificationEvent,
                                     value.event_parameter,
                                     value.event_parameters_length );
    }

    if( !isNotificationRequested ) {
        if( node->isWakeup() ) {
            if( isWokenup ) {
                if( is_self_node_sis_or_primary() ) {
                    node->updateWakeUpInterval();
                }
                m_parentMesh.requestNodeValues( nodeId, false );
            }
            else {
                node->setIsWokenUp( false );
            }
        }
    }
}

void MonitoringBaseHandler::onNodeCmdMultilevelSwitch( const uint8_t nodeId,
                                                       const uint8_t channelId,
                                                       const uint8_t currentValue,
                                                       const Uint8Opt targetValue,
                                                       const SecondsOpt targetDuration ) {
    ZNode* node = m_parentMesh.onFindNode( nodeId );
    if( node == nullptr ) {
        ZNET_LOGE( "%s could not find node info  Id = %d\n", __func__, nodeId );
        return;
    }

    updateReachableState( *node );

    ZNode::ClassPtr ccTemp = node->getCommandClass( channelId, ZNET_COMMAND_CLASS_SWITCH_MULTILEVEL );
    if( ccTemp ) {
        ZCc0x26SwitchMultiLevel* cc = static_cast<ZCc0x26SwitchMultiLevel*>( ccTemp.get() );
        if( cc != nullptr && cc->getId() == ZNET_COMMAND_CLASS_SWITCH_MULTILEVEL ) {
            IZNetOwner& owner = m_parentMesh.getOwner();
            if( targetValue && targetDuration ) {
                ZNET_LOGD( "%s val (%d -> %d), duration (%ds) on target(%d)\n",
                           __func__,
                           currentValue,
                           *targetValue,
                           targetDuration->count(),
                           cc->getTargetLevel() );
            }
            else {
                ZNET_LOGD( "%s val %d on target(%d)\n", __func__, currentValue, cc->getTargetLevel() );
            }

            owner.onUpdateValue(
                nodeId, ZNET_COMMAND_CLASS_SWITCH_MULTILEVEL, channelId, currentValue, targetValue, targetDuration );
        }
    }
}

void MonitoringBaseHandler::onNodeCmdBarrierOperator( const uint8_t nodeId,
                                                      const uint8_t channelId,
                                                      const uint8_t value ) {
    ZNode* node = m_parentMesh.onFindNode( nodeId );
    if( node == nullptr ) {
        ZNET_LOGE( "%s could not find node info  Id = %d\n", __func__, nodeId );
        return;
    }

    updateReachableState( *node );

    ZNode::ClassPtr ccTemp = node->getCommandClass( channelId, ZNET_COMMAND_CLASS_BARRIER_OPERATOR );
    if( ccTemp ) {
        ZCc0x66BarrierOperator* cc = static_cast<ZCc0x66BarrierOperator*>( ccTemp.get() );
        if( cc != nullptr && cc->getId() == ZNET_COMMAND_CLASS_BARRIER_OPERATOR ) {
            IZNetOwner& owner = m_parentMesh.getOwner();
            ZNET_LOGD( "%s val(%d)\n", __func__, value );
            std::string tokenValue =
                barrier_operator_state_converter::typeToString( static_cast<BarrierOperatorState>( value ) );
            owner.onUpdateTokenValue( nodeId, ZNET_COMMAND_CLASS_BARRIER_OPERATOR, channelId, tokenValue );
        }
    }
}

void MonitoringBaseHandler::onNodeCmdBarrierOperatorSignal( const uint8_t nodeId,
                                                            const uint8_t channelId,
                                                            const uint8_t signalId,
                                                            const uint8_t state ) {
    ZNode* node = m_parentMesh.onFindNode( nodeId );
    if( node == nullptr ) {
        ZNET_LOGE( "%s could not find node info  Id = %d\n", __func__, nodeId );
        return;
    }

    updateReachableState( *node );

    ZNode::ClassPtr ccTemp = node->getCommandClass( channelId, ZNET_COMMAND_CLASS_BARRIER_OPERATOR );
    if( ccTemp ) {
        ZCc0x66BarrierOperator* cc = static_cast<ZCc0x66BarrierOperator*>( ccTemp.get() );
        if( cc != nullptr && cc->getId() == ZNET_COMMAND_CLASS_BARRIER_OPERATOR ) {
            IZNetOwner& owner = m_parentMesh.getOwner();
            ZNET_LOGD( "%s id(%d) val(%d)\n", __func__, signalId, state );
            owner.onUpdateBarrierSignalValue( nodeId, channelId, signalId, state );
        }
    }
}

void MonitoringBaseHandler::onNodeCmdAlarmSensor( const uint8_t nodeId,
                                                  const uint8_t channelId,
                                                  const uint8_t sensorType,
                                                  const uint8_t value,
                                                  const uint16_t seconds ) {
    ZNET_LOGD( "%s type: %u, value: %u, seconds: %u\n", __func__, sensorType, value, seconds );

    ZNode* node = m_parentMesh.onFindNode( nodeId );
    if( node == nullptr ) {
        ZNET_LOGE( "%s could not find node info  Id = %d\n", __func__, nodeId );
        return;
    }

    updateReachableState( *node );

    IZNetOwner& owner = m_parentMesh.getOwner();
    owner.onUpdateValueAlarmSensor( nodeId, channelId, static_cast<AlarmSensorType>( sensorType ), value, seconds );
}

void MonitoringBaseHandler::onNodeCmdBinarySensor( const uint8_t nodeId,
                                                   const uint8_t channelId,
                                                   const uint8_t value,
                                                   const uint8_t sensorType ) {
    ZNET_LOGD( "%s type: %u, value: %u\n", __func__, sensorType, value );

    ZNode* node = m_parentMesh.onFindNode( nodeId );
    if( node == nullptr ) {
        ZNET_LOGE( "%s could not find node info  Id = %d\n", __func__, nodeId );
        return;
    }

    updateReachableState( *node );

    IZNetOwner& owner = m_parentMesh.getOwner();
    owner.onUpdateValueBinarySensor( nodeId, ZNET_COMMAND_CLASS_SENSOR_BINARY, channelId, sensorType, value );
}

void MonitoringBaseHandler::onNodeCmdSensorMultilevel( znet_node_id_t nodeId,
                                                       znet_node_channel_id_t channelId,
                                                       const znet_multilevel_sensor_report_t* value ) {
    ZNET_LOGD( "%s "
               "type: %u, "
               "scale: %u, "
               "value: %i, "
               "precision: %u\n",
               __func__,
               value->type,
               value->scale,
               value->value,
               value->precision );

    ZNode* node = m_parentMesh.onFindNode( nodeId );
    if( node == nullptr ) {
        ZNET_LOGE( "%s could not find node info  Id = %d\n", __func__, nodeId );
        return;
    }

    updateReachableState( *node );

    auto ccTemp = node->getCommandClass<ZCc0x31SensorMultilevel>( channelId );
    if( ccTemp ) {
        const auto sensorType = static_cast<MultilevelSensor::Type>( value->type );

        if( ccTemp->isValidSensorType( sensorType ) ) {
            auto& owner = m_parentMesh.getOwner();
            owner.onUpdateValueMultilevelSensor( nodeId,
                                                 ZNET_COMMAND_CLASS_SENSOR_MULTILEVEL,
                                                 channelId,
                                                 sensorType,
                                                 value->scale,
                                                 utils::valueToFloat( value->value, value->precision ) );
        }
        else {
            assert( false );
        }
    }
}

void MonitoringBaseHandler::onNodeCmdMeter( const znet_node_id_t nodeId,
                                            const znet_node_channel_id_t channelId,
                                            const meter::MeterType type,
                                            const meter::Scale scale,
                                            const int32_t value,
                                            const uint8_t precision ) {
    ZNode* node = m_parentMesh.onFindNode( nodeId );
    if( node == nullptr ) {
        ZNET_LOGE( "%s could not find node info  Id = %d\n", __func__, nodeId );
        return;
    }

    updateReachableState( *node );
    m_parentMesh.getOwner().onUpdateFloatValue(
        nodeId, static_cast<uint8_t>( ZNET_COMMAND_CLASS_METER ), channelId, type, scale, value, precision );
}

void MonitoringBaseHandler::onApplicationNodeUpdate( const znet_nodeinfo_t* nodeInfo ) {
    if( nodeInfo == nullptr ) {
        ZNET_LOGE( "wrong node info\n" );
        return;
    }

    ZNode* node = m_parentMesh.onFindNode( nodeInfo->node_id );
    if( node == nullptr ) {
        ZNET_LOGE( "%s could not find node info  Id = %d\n", __func__, nodeInfo->node_id );
        return;
    }

    bool is_wokenup = node->isWokenUp();
    updateReachableState( *node );

    if( !node->isWakeup() || is_wokenup ) {
        m_parentMesh.getOwner().onNif( nodeInfo->node_id );
    }
    else {
        node->setIsWokenUp( false );
    }
}

void MonitoringBaseHandler::onWakeupNotification( uint8_t nodeId ) {
    ZNode* node = m_parentMesh.onFindNode( nodeId );
    if( node == nullptr ) {
        ZNET_LOGE( "%s could not find node info  Id = %d\n", __func__, nodeId );
        return;
    }

    if( node->isRediscoverRequired() ) {
        m_parentMesh.interviewNode( nodeId );
        return;
    }

    updateReachableState( *node, true );
    node->setIsWokenUp( true );
    if( is_self_node_sis_or_primary() ) {
        node->updateWakeUpInterval();
    }
    m_parentMesh.requestNodeValues( nodeId, false );
}

void MonitoringBaseHandler::onWakeUpResult( const znet_node_id_t nodeId,
                                            const uint32_t interval,
                                            const znet_node_id_t targetNodeId ) {
    ZNode* node = m_parentMesh.onFindNode( nodeId );
    if( node == nullptr ) {
        ZNET_LOGE( "%s could not find node info  Id = %d\n", __func__, nodeId );
        return;
    }

    if( auto cc = node->getCommandClass<ZCc0x84WakeUp>( ZNET_CHANNEL_ID_ROOT ) ) {
        const auto prevInterval = cc->getInterval();
        cc->onIntervalReceived( interval, targetNodeId );
        bool isChanged = prevInterval != cc->getInterval();
        if( isChanged ) {
            ZNET_LOGW( "%s for node %u has been changed %u -> %u by report. It looks interval was set "
                       "incorrectly or device has changed it by itself\n",
                       __func__,
                       prevInterval,
                       cc->getInterval() );
        }

        m_parentMesh.getOwner().onWakeupIntervalUpdated( nodeId, Seconds( cc->getInterval() ), isChanged );
    }

    updateReachableState( *node );
}

void MonitoringBaseHandler::onCentralSceneConfiguration( const uint8_t nodeId,
                                                         const uint8_t channelId,
                                                         const uint8_t value ) {
    // TODO: implement sending of configuration value
}

void MonitoringBaseHandler::onNodeCmdSceneActuatorConfiguration( const uint8_t nodeId,
                                                                 const uint8_t channelId,
                                                                 const uint8_t sceneId,
                                                                 const uint8_t level,
                                                                 const uint8_t dimmingDuration ) {
    // TODO: implement saving of configuration value
}

void MonitoringBaseHandler::onNodeCmdSceneControllerConfiguration( const uint8_t nodeId,
                                                                   const uint8_t channelId,
                                                                   const uint8_t groupId,
                                                                   const uint8_t sceneId,
                                                                   const uint8_t dimmingDuration ) {
    // TODO: implement saving of configuration value
}

void MonitoringBaseHandler::onCentralSceneNotification( const uint8_t nodeId,
                                                        const uint8_t sequence_number,
                                                        const uint8_t slow_refresh,
                                                        const uint8_t key_attribute,
                                                        const uint8_t scene_number ) {
    ZNode* node = m_parentMesh.onFindNode( nodeId );
    if( node == nullptr ) {
        ZNET_LOGE( "%s could not find node info  Id = %d\n", __func__, nodeId );
        return;
    }

    updateReachableState( *node );

    auto attribute =
        central_scenes_attributes_converter::stateToString( static_cast<CentralSceneAttributes>( key_attribute ) );

    IZNetOwner& owner = m_parentMesh.getOwner();
    owner.onUpdateSceneValue( nodeId, ZNET_COMMAND_CLASS_CENTRAL_SCENE, 0, sequence_number, scene_number, attribute );
}

void MonitoringBaseHandler::onSceneActivationSetResult( const uint8_t nodeId, const uint8_t scene_number ) {
    ZNode* node = m_parentMesh.onFindNode( nodeId );
    if( node == nullptr ) {
        ZNET_LOGE( "%s could not find node info  Id = %d\n", __func__, nodeId );
        return;
    }

    updateReachableState( *node );

    IZNetOwner& owner = m_parentMesh.getOwner();
    owner.onSceneActivated( nodeId, ZNET_COMMAND_CLASS_SCENE_ACTIVATION, 0, scene_number );
}

void MonitoringBaseHandler::onSimpleAvControlSetResult( const uint8_t nodeId,
                                                        const uint8_t channelId,
                                                        const znet_simple_av_set_report_t* value ) {
    ZNode* node = m_parentMesh.onFindNode( nodeId );
    if( node == nullptr ) {
        ZNET_LOGE( "%s could not find node info  Id = %d\n", __func__, nodeId );
        return;
    }

    updateReachableState( *node );

    std::vector<uint16_t> commands( value->commands, value->commands + value->commands_count );
    auto attribute = central_scenes_attributes_converter::stateToString(
        static_cast<CentralSceneAttributes>( value->key_attribute ) );

    IZNetOwner& owner = m_parentMesh.getOwner();
    owner.onSimpleAvControlSet( nodeId, channelId, value->sequence_id, attribute, std::move( commands ) );
}

void MonitoringBaseHandler::onConfigurationReport( const uint8_t nodeId,
                                                   const uint8_t channelId,
                                                   const uint16_t configParamNum,
                                                   const uint8_t configParamSize,
                                                   const uint32_t configParamValue ) {
    ZNode* node = m_parentMesh.onFindNode( nodeId );
    if( node == nullptr ) {
        ZNET_LOGE( "%s could not find node info  Id = %d\n", __func__, nodeId );
        return;
    }

    updateReachableState( *node );

    IZNetOwner& owner = m_parentMesh.getOwner();
    owner.onConfigurationReport( nodeId, channelId, configParamNum, configParamSize, configParamValue );
}

void MonitoringBaseHandler::onConfigurationRequestStatus( const uint8_t nodeId,
                                                          const uint8_t channelId,
                                                          const uint8_t configParamNum,
                                                          RequestStatus::Enum status ) {
    ZNode* node = m_parentMesh.onFindNode( nodeId );
    if( node == nullptr ) {
        ZNET_LOGE( "%s could not find node info  Id = %d\n", __func__, nodeId );
        return;
    }

    IZNetOwner& owner = m_parentMesh.getOwner();
    owner.onConfigurationRequestStatus( nodeId, configParamNum, status );
}

void MonitoringBaseHandler::onAssociationResult( const uint8_t nodeId,
                                                 const uint8_t channelId,
                                                 const znet_association_report_t* value ) {
    ZNode* node = m_parentMesh.onFindNode( nodeId );
    if( node == nullptr ) {
        ZNET_LOGE( "%s could not find node info  Id = %d\n", __func__, nodeId );
        return;
    }

    updateReachableState( *node );

    ZNode::ClassPtr ccTemp = node->getCommandClass( channelId, ZNET_COMMAND_CLASS_ASSOCIATION );
    if( ccTemp ) {
        ZCc0x85Association* cc = static_cast<ZCc0x85Association*>( ccTemp.get() );
        if( cc != nullptr && cc->getId() == ZNET_COMMAND_CLASS_ASSOCIATION ) {
            cc->onGroupInfo( value );

            IZNetOwner& owner = m_parentMesh.getOwner();
            const auto* groupNodes = cc->getGroupNodes( value->grouping_id );
            owner.onAssociationResult(
                nodeId, channelId, value->grouping_id, groupNodes ? *groupNodes : std::vector<uint8_t>() );
        }
    }
}

void MonitoringBaseHandler::onMultichannelAssociationResult( const uint8_t nodeId,
                                                             const uint8_t channelId,
                                                             const znet_multichannel_association_report_t* value ) {
    ZNode* node = m_parentMesh.onFindNode( nodeId );
    if( node == nullptr ) {
        ZNET_LOGE( "%s could not find node info  Id = %d\n", __func__, nodeId );
        return;
    }

    updateReachableState( *node );

    ZNode::ClassPtr ccTemp = node->getCommandClass( channelId, ZNET_COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION );
    if( ccTemp ) {
        ZCc0x8eMultiChannelAssociation* cc = static_cast<ZCc0x8eMultiChannelAssociation*>( ccTemp.get() );
        if( cc != nullptr && cc->getId() == ZNET_COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION ) {
            cc->onGroupInfo( value );
        }
    }

    IZNetOwner& owner = m_parentMesh.getOwner();

    std::vector<std::pair<uint8_t, uint8_t> > nodes;
    for( int i = 0; i < value->end_pints_count; ++i ) {
        nodes.push_back( std::make_pair( value->end_pints[i].node_id, value->end_pints[i].channel_id ) );
    }

    owner.onMultichannelAssociationResult( nodeId, channelId, value->grouping_id, nodes );
}

void MonitoringBaseHandler::onAssociationSupportedGroupingResult( const uint8_t nodeId,
                                                                  const uint8_t channelId,
                                                                  const uint8_t count ) {
    ZNode* node = m_parentMesh.onFindNode( nodeId );
    if( node == nullptr ) {
        ZNET_LOGE( "%s could not find node info  Id = %d\n", __func__, nodeId );
        return;
    }

    updateReachableState( *node );

    IZNetOwner& owner = m_parentMesh.getOwner();
    owner.onAssociationSupportedGroupingResult( nodeId, count );
}

void MonitoringBaseHandler::onThermostatModeResult( uint8_t nodeId,
                                                    uint8_t channelId,
                                                    const znet_thermostat_mode_report_t& value ) {
    ZNode* node = m_parentMesh.onFindNode( nodeId );
    if( node == nullptr ) {
        ZNET_LOGE( "%s could not find node info  Id = %d\n", __func__, nodeId );
        return;
    }

    updateReachableState( *node );

    m_parentMesh.getOwner().onUpdateThermostatModeValue(
        nodeId,
        channelId,
        value.mode,
        value.dcount ? std::vector<uint8_t>{ value.manuf_data, value.manuf_data + value.dcount } :
                       ManufacturerSpecificDataOpt{} );
}

void MonitoringBaseHandler::onThermostatOperatingStateResult( const uint8_t nodeId, const uint8_t value ) {
    ZNode* node = m_parentMesh.onFindNode( nodeId );
    if( node == nullptr ) {
        ZNET_LOGE( "%s could not find node info  Id = %d\n", __func__, nodeId );
        return;
    }

    updateReachableState( *node );

    IZNetOwner& owner = m_parentMesh.getOwner();
    owner.onUpdateValueThermostatOperatingState( nodeId,
                                                 0 /*channelId*/,
                                                 ZNET_COMMAND_CLASS_THERMOSTAT_OPERATING_STATE,
                                                 static_cast<ThermostatOperatingState>( value ) );
}

void MonitoringBaseHandler::onThermostatOperatingStateLoggingResult( const uint8_t nodeId,
                                                                     const uint8_t rep_to_follows,
                                                                     const uint16_t data_count,
                                                                     const uint8_t* data ) {
    ZNode* node = m_parentMesh.onFindNode( nodeId );
    if( node == nullptr ) {
        ZNET_LOGE( "%s could not find node info  Id = %d\n", __func__, nodeId );
        return;
    }

    updateReachableState( *node );

    // TODO: implement whith "What happend" task
}

void MonitoringBaseHandler::onThermostatFanModeResult( const uint8_t nodeId, const uint8_t value ) {
    ZNode* node = m_parentMesh.onFindNode( nodeId );
    if( node == nullptr ) {
        ZNET_LOGE( "%s could not find node info  Id = %d\n", __func__, nodeId );
        return;
    }

    updateReachableState( *node );

    IZNetOwner& owner = m_parentMesh.getOwner();
    owner.onUpdateValue( nodeId, ZNET_COMMAND_CLASS_THERMOSTAT_FAN_MODE, 0, value );
}

void MonitoringBaseHandler::onThermostatFanStateResult( const uint8_t nodeId, const uint8_t value ) {
    ZNode* node = m_parentMesh.onFindNode( nodeId );
    if( node == nullptr ) {
        ZNET_LOGE( "%s could not find node info  Id = %d\n", __func__, nodeId );
        return;
    }

    updateReachableState( *node );

    IZNetOwner& owner = m_parentMesh.getOwner();
    owner.onUpdateValue( nodeId, ZNET_COMMAND_CLASS_THERMOSTAT_FAN_STATE, 0, value );
}

void MonitoringBaseHandler::onThermostatSetpointResult( const uint8_t nodeId,
                                                        const uint8_t channelId,
                                                        const uint8_t type,
                                                        const uint8_t scale,
                                                        const uint8_t size,
                                                        const uint8_t precision,
                                                        const int32_t sp_value,
                                                        const bool isSetpointRequested ) {
    ZNode* node = m_parentMesh.onFindNode( nodeId );
    if( node == nullptr ) {
        ZNET_LOGE( "%s could not find node info  Id = %d\n", __func__, nodeId );
        return;
    }

    updateReachableState( *node );

    auto ccTemp = node->getCommandClass<ZCc0x43ThermostatSetpoint>( channelId );
    if( ccTemp ) {
        m_parentMesh.getOwner().onUpdateValueThermostatSetpoint(
            nodeId, channelId, type, scale, utils::valueToFloat( sp_value, precision ), isSetpointRequested );
    }
}

void MonitoringBaseHandler::onThermostatSetbackResult( const znet_node_id_t nodeId,
                                                       const uint8_t channelId,
                                                       const thermostat_setback::OverrideType type,
                                                       const thermostat_setback::Mode mode,
                                                       const thermostat_setback::SetbackValueOpt valueOpt ) {
    ZNode* node = m_parentMesh.onFindNode( nodeId );
    if( node == nullptr ) {
        ZNET_LOGE( "%s could not find node info  Id = %d\n", __func__, nodeId );
        return;
    }

    updateReachableState( *node );

    IZNetOwner& owner = m_parentMesh.getOwner();
    owner.onUpdateThermostatSetbackValue( nodeId, channelId, type, mode, valueOpt );
}

void MonitoringBaseHandler::onNodeCmdUserCodeReport( const znet_node_id_t nodeId,
                                                     const znet_node_channel_id_t channelId,
                                                     const znet_user_code_report_t* user_code_reports,
                                                     uint8_t count,
                                                     UserCodeNumberOpt nextUserCode,
                                                     UserCode::UserPollStatus pollingStatus ) {
    auto node = m_parentMesh.onFindNode( nodeId );
    if( node == nullptr ) {
        ZNET_LOGE( "%s could not find node info  Id = %d\n", __func__, nodeId );
        return;
    }

    updateReachableState( *node );

    if( auto cc = node->getCommandClass<ZCc0x63UserCode>( channelId ) ) {
        IZNetOwner& owner = m_parentMesh.getOwner();
        znet_supported_users_t index = 0;
        UserCode::UserCodeRanges ranges;

        auto onUserCodeUpdated = [&]( znet_supported_users_t userId, UserCode::UserIDStatus status, std::string code ) {
            cc->getUserIdsCache().onUserCodeUpdated( userId, status );

            if( !ranges.empty() && ranges.back().lastId != 0 && ranges.back().lastId + 1 == userId &&
                ranges.back().status == status && ranges.back().code == code ) {
                ranges.back().lastId = userId;
            }
            else {
                ranges.push_back( { userId, userId, status, std::move( code ) } );
            }
        };

        for( znet_supported_users_t userId = user_code_reports[0].user_id;
             userId <= user_code_reports[count - 1].user_id;
             ++userId ) {
            if( userId == user_code_reports[index].user_id ) {
                const auto userCode = reinterpret_cast<const char*>( user_code_reports[index].user_code );
                onUserCodeUpdated(
                    userId,
                    static_cast<UserCode::UserIDStatus>( user_code_reports[index].user_id_status ),
                    std::string( userCode, strnlen( userCode, user_code_reports[index].user_code_size ) ) );
                ++index;
            }
            else {
                onUserCodeUpdated( userId, UserCode::UserIDStatus::Available, std::string() );
            }
        }

        if( nextUserCode ) {
            const auto maxNextUserCode = static_cast<znet_supported_users_t>( cc->getSupportedUsersCount() + 1 );
            nextUserCode = *nextUserCode == 0 ? maxNextUserCode : std::min( *nextUserCode, maxNextUserCode );

            for( znet_supported_users_t userId = user_code_reports[count - 1].user_id + 1; userId < *nextUserCode;
                 ++userId ) {
                onUserCodeUpdated( userId, UserCode::UserIDStatus::Available, std::string() );
            }
        }

        owner.onUserCodeUpdated(
            nodeId,
            channelId,
            std::move( ranges ),
            cc->getSupportedUsersCount(),
            pollingStatus == UserCode::UserPollStatus::Finished ? UserCode::UserPollStatus::Polling : pollingStatus );
    }
}

void MonitoringBaseHandler::onNodeMasterCodeReport( znet_node_id_t nodeId,
                                                    znet_node_channel_id_t channelId,
                                                    uint8_t codeLength,
                                                    const uint8_t* masterCode ) {
    ZNode* node = m_parentMesh.onFindNode( nodeId );
    if( node == nullptr ) {
        ZNET_LOGE( "%s could not find node info  Id = %d\n", __func__, nodeId );
        return;
    }

    m_parentMesh.getOwner().onMasterCodeUpdated(
        nodeId, channelId, std::string( reinterpret_cast<const char*>( masterCode ), codeLength ) );
}

void MonitoringBaseHandler::onNodeUserCodesKeypadModeReport( znet_node_id_t nodeId,
                                                             znet_node_channel_id_t channelId,
                                                             znet_user_code_keypad_mode_t keypadMode ) {
    ZNode* node = m_parentMesh.onFindNode( nodeId );
    if( node == nullptr ) {
        ZNET_LOGE( "%s could not find node info  Id = %d\n", __func__, nodeId );
        return;
    }

    m_parentMesh.getOwner().onUserCodesKeypadModeUpdated(
        nodeId, channelId, static_cast<UserCode::KeypadMode>( keypadMode ) );
}

void MonitoringBaseHandler::onNodeScheduleEntryLockWeekDayReport( znet_node_id_t nodeId,
                                                                  znet_node_channel_id_t channelId,
                                                                  const znet_week_day_schedule_t& value ) {
    auto node = m_parentMesh.onFindNode( nodeId );
    if( !node ) {
        ZNET_LOGE( "%s could not find node info  Id = %d\n", __func__, nodeId );
        return;
    }

    m_parentMesh.getOwner().onUpdateScheduleEntryLockWeekDay( nodeId, channelId, value );

    updateReachableState( *node );
}

void MonitoringBaseHandler::onNodeScheduleEntryLockYearDayReport( znet_node_id_t nodeId,
                                                                  znet_node_channel_id_t channelId,
                                                                  const znet_year_day_schedule_t& value ) {
    auto node = m_parentMesh.onFindNode( nodeId );
    if( !node ) {
        ZNET_LOGE( "%s could not find node info  Id = %d\n", __func__, nodeId );
        return;
    }

    m_parentMesh.getOwner().onUpdateScheduleEntryLockYearDay( nodeId, channelId, value );

    updateReachableState( *node );
}

void MonitoringBaseHandler::onNodeScheduleEntryLockDailyRepeatingReport(
    znet_node_id_t nodeId,
    znet_node_channel_id_t channelId,
    const znet_daily_repeating_schedule_t& value ) {
    auto node = m_parentMesh.onFindNode( nodeId );
    if( !node ) {
        ZNET_LOGE( "%s could not find node info  Id = %d\n", __func__, nodeId );
        return;
    }

    m_parentMesh.getOwner().onUpdateScheduleEntryLockDailyRepeating( nodeId, channelId, value );

    updateReachableState( *node );
}

void MonitoringBaseHandler::onNodeSoundSwitchConfigurationReport( znet_node_id_t nodeId,
                                                                  znet_node_channel_id_t channelId,
                                                                  const znet_cmd_sound_switch_configuration_t& value ) {
    auto node = m_parentMesh.onFindNode( nodeId );
    if( !node ) {
        ZNET_LOGE( "%s could not find node info  Id = %d\n", __func__, nodeId );
        return;
    }

    m_parentMesh.getOwner().onUpdateSoundSwitchConfiguration( nodeId, channelId, value );

    updateReachableState( *node );
}

void MonitoringBaseHandler::onNodeSoundSwitchTonePlayReport( znet_node_id_t nodeId,
                                                             znet_node_channel_id_t channelId,
                                                             const znet_cmd_sound_switch_tone_play_t& value ) {
    auto node = m_parentMesh.onFindNode( nodeId );
    if( !node ) {
        ZNET_LOGE( "%s could not find node info  Id = %d\n", __func__, nodeId );
        return;
    }

    m_parentMesh.getOwner().onUpdateSoundSwitchTonePlay( nodeId, channelId, value );

    updateReachableState( *node );
}

void MonitoringBaseHandler::onNodeCmdHail( const znet_node_id_t nodeId ) {
    ZNode* node = m_parentMesh.onFindNode( nodeId );
    if( node == nullptr ) {
        ZNET_LOGE( "%s could not find node info  Id = %d\n", __func__, nodeId );
        return;
    }

    m_parentMesh.getOwner().onHail( nodeId );
}

void MonitoringBaseHandler::onIndicatorIdentify() {
    m_parentMesh.getOwner().onIndicatorIdentify();
}

void MonitoringBaseHandler::onNodeCmdAntiTheftReport( znet_node_id_t nodeId,
                                                      znet_node_channel_id_t channelId,
                                                      const znet_cmd_anti_theft_report_t& report ) {
    m_parentMesh.getOwner().onAntiTheftStateUpdated(
        nodeId, channelId, ZNET_COMMAND_CLASS_ANTITHEFT, AntiTheftInfo::makeFromReport( report ) );
}

void MonitoringBaseHandler::onNodeCmdAntiTheftUnlockStateReport(
    znet_node_id_t nodeId,
    znet_node_channel_id_t channelId,
    const znet_cmd_anti_theft_unlock_state_report_t& report ) {
    m_parentMesh.getOwner().onAntiTheftStateUpdated(
        nodeId, channelId, ZNET_COMMAND_CLASS_ANTITHEFT_UNLOCK, AntiTheftInfo::makeFromReport( report ) );
}

}  // namespace znet
