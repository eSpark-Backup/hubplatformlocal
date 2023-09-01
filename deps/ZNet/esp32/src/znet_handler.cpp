#include <znet_handler.hpp>
#include <znet/znet.h>
#include <znet_impl.hpp>
#include <znet/cc_0x7a_firmware_update.hpp>

extern "C" {
#include <znet_log.h>
}

namespace znet {

ZNetHandler::ZNetHandler( ZNetImpl& parent )
: m_parentMesh( parent ) {}

void ZNetHandler::initialize() {}

bool ZNetHandler::onLoop() {
    return true;
}

void ZNetHandler::finalize( ErrorOpt ) {}

// Incorrect:use finalize instead
void ZNetHandler::onDeactivated() {}

void ZNetHandler::setTimeout( uint64_t timeout_ms ) {
    ZNET_LOGD( "%s  timeout = %d\n", __func__, timeout_ms );
    if( timeout_ms ) {
        m_timeOut = std::chrono::system_clock::now() + std::chrono::milliseconds( timeout_ms );
    }
    else {
        m_timeOut = std::chrono::system_clock::from_time_t( 0 );
    }
}

void ZNetHandler::onNodeAdd( const znet_nodeinfo_t* node_info ) {}
void ZNetHandler::onNodeAddSecurityFailed() {}
void ZNetHandler::onNodeAddSecurityKeysRequested( znet_S2_keys_t ) {}
void ZNetHandler::onNodeAddSecurityPubKeyRequested( const znet_S2_pub_key_t* ) {}
void ZNetHandler::onNodeAddSecurityKeysGranted( znet_S2_keys_t ) {}
void ZNetHandler::onNodeAddSecurityPubKeyGranted( const znet_S2_pub_key_t* ) {}
void ZNetHandler::onNodeAddSecurityDone( SecurityMode::Class ) {}
void ZNetHandler::onNodeLearn( const znet_node_id_t destNodeId ) {}
void ZNetHandler::onNodeList( const uint8_t* nodesList ) {}
void ZNetHandler::onNodeLearnBreak(::znet_error_t, ::znet_node_id_t ) {}
void ZNetHandler::onNodeRemInvoked() {}
void ZNetHandler::onNodeRem( znet_error_t, const znet_nodeinfo_t* ) {}
void ZNetHandler::onNodeRemBreak( znet_error_t, const znet_nodeinfo_t* ) {}
void ZNetHandler::onNodeNoOperation( const znet_node_id_t nodeId, znet_error_t error ) {
    auto updateReachable = [&]( bool isReachable ) {
        ZNode* node = m_parentMesh.onFindNode( nodeId );
        if( node == nullptr ) {
            ZNET_LOGE( "%s could not find node info  Id = %d\n", __func__, nodeId );
            return;
        }
        if( isReachable || !node->isWakeup( true ) ||
            ( node->isWakeup() && node->isWokenUp() && is_self_node_sis_or_primary() ) ) {
            ZNET_LOGI( "Set node %u reachable %s by nop result\n", nodeId, isReachable ? "true" : "false" );
            m_parentMesh.setReachableState( nodeId, isReachable );
        }
    };

    if( error == ZNET_ERR_REQUEST_SENT_OK ) {
        updateReachable( true );
    }
    else if( error == ZNET_ERR_UNREACH ) {
        updateReachable( false );
    }
}
void ZNetHandler::onForcedRequestResult() {}

void ZNetHandler::onNodeCmdSecuritySupported( const uint8_t nodeId, const znet_security_supported_report_t* value ) {}

void ZNetHandler::onNodeCmdCommandVersion( const uint8_t nodeId, const uint8_t classId, const uint8_t version ) {}
void ZNetHandler::onNodeCmdVersion( znet_node_id_t nodeId, const znet_version_report_t* versionReport ) {}
void ZNetHandler::onNodeCmdVersionCapabilities( znet_node_id_t nodeId,
                                                const znet_version_capabilities_report_t* value ) {}
void ZNetHandler::onNodeCmdSoftwareVersion( znet_node_id_t nodeId, const znet_software_version_report_t* value ) {}

void ZNetHandler::onNodeCmdZWavePlusInfo( const uint8_t nodeId, const znet_zwaveplus_info_report_t* value ) {}
void ZNetHandler::onNodeCmdManufacturerSpecific( const uint8_t nodeId,
                                                 const uint16_t manufacturerId,
                                                 const uint16_t productType,
                                                 const uint16_t productId ) {}

void ZNetHandler::onNodeCmdAlarmSensor( const uint8_t nodeId,
                                        const uint8_t channelId,
                                        const uint8_t sensorType,
                                        const uint8_t value,
                                        const uint16_t seconds ) {}

void ZNetHandler::onNodeCmdAlarmSensorSupported( const uint8_t nodeId,
                                                 const uint8_t channelId,
                                                 const uint8_t* data,
                                                 const uint8_t len ) {}

void ZNetHandler::onNodeCmdBinarySensor( const uint8_t nodeId,
                                         const uint8_t channelId,
                                         const uint8_t value,
                                         const uint8_t sensorType ) {}

void ZNetHandler::onNodeCmdBinarySensorSupported( const uint8_t nodeId,
                                                  const uint8_t channelId,
                                                  const uint8_t* data,
                                                  const uint8_t len ) {}

void ZNetHandler::onNodeCmdSensorMultilevelSupportedSensor( const uint8_t nodeId,
                                                            const uint8_t channelId,
                                                            const uint8_t* data,
                                                            uint8_t len ) {}

void ZNetHandler::onNodeCmdSensorMultilevelSupportedScale( znet_node_id_t nodeId,
                                                           znet_node_channel_id_t channelId,
                                                           uint8_t sensor_type,
                                                           uint8_t scale_mask ) {}

void ZNetHandler::onNodeCmdSensorMultilevel( znet_node_id_t nodeId,
                                             znet_node_channel_id_t channelId,
                                             const znet_multilevel_sensor_report_t* value ) {}

void ZNetHandler::onNodeCmdMultilevelSwitchSupported( znet_node_id_t nodeId,
                                                      znet_node_channel_id_t channelId,
                                                      uint8_t primary_type,
                                                      uint8_t secondary_type ) {}

void ZNetHandler::onNodeCmdMeterSupported( const znet_node_id_t nodeId,
                                           const znet_node_channel_id_t channelId,
                                           const bool resetSupported,
                                           const meter::MeterType type,
                                           const uint8_t scalesBitmaskSize,
                                           const uint8_t* scalesBitmask ) {}

void ZNetHandler::onNodeCmdSceneActuatorConfiguration( const uint8_t nodeId,
                                                       const uint8_t channelId,
                                                       const uint8_t sceneId,
                                                       const uint8_t level,
                                                       const uint8_t dimmingDuration ) {}

void ZNetHandler::onNodeCmdSceneControllerConfiguration( const uint8_t nodeId,
                                                         const uint8_t channelId,
                                                         const uint8_t groupId,
                                                         const uint8_t sceneId,
                                                         const uint8_t dimmingDuration ) {}

void ZNetHandler::onCentralSceneConfiguration( const uint8_t nodeId, const uint8_t channelId, const uint8_t sr_value ) {
}

void ZNetHandler::onNodeCmdCentralSceneSupported( const uint8_t nodeId,
                                                  const uint8_t channelId,
                                                  const uint8_t scenes_num,
                                                  const uint8_t data_size,
                                                  const uint8_t identical,
                                                  const uint8_t slow_refresh,
                                                  const uint8_t* data ) {}

void ZNetHandler::onCentralSceneNotification( const uint8_t nodeId,
                                              const uint8_t sequence_number,
                                              const uint8_t slow_refresh,
                                              const uint8_t key_attribute,
                                              const uint8_t scene_number ) {}

void ZNetHandler::onSceneActivationSetResult( const uint8_t nodeId, const uint8_t scene_number ) {}

void ZNetHandler::onNodeCmdBasic( const uint8_t nodeId,
                                  const uint8_t channelId,
                                  const uint8_t currentValue,
                                  uint8_t targetValue ) {}

void ZNetHandler::onNodeCmdBasicSet( const uint8_t nodeId,
                                     const uint8_t channelId,
                                     const uint8_t currentValue,
                                     uint8_t targetValue ) {}

void ZNetHandler::onNodeCmdBatteryLevel( const uint8_t nodeId, const uint8_t channelId, const uint8_t value ) {}

void ZNetHandler::onNodeCmdBinarySwitch( const uint8_t nodeId,
                                         const uint8_t channelId,
                                         const uint8_t value,
                                         const Uint8Opt targetValue,
                                         const SecondsOpt duration ) {}

void ZNetHandler::onNodeCmdClock( const uint8_t nodeId, const uint8_t channelId, const znet_clock_value_t* value ) {}

void ZNetHandler::onNodeCmdTimeParameters( const uint8_t nodeId,
                                           const uint8_t channelId,
                                           const znet_cmd_time_parameters_report_t* value ) {}

void ZNetHandler::onNodeCmdColorSwitch( const uint8_t nodeId,
                                        const uint8_t channelId,
                                        const ColorComponentID componentId,
                                        const uint8_t value,
                                        const utils::optional<uint8_t> targetValue,
                                        const SecondsOpt targetDuration ) {}

void ZNetHandler::onNodeCmdColorSwitchSupported( const uint8_t nodeId,
                                                 const uint8_t channelId,
                                                 const uint8_t masksCount,
                                                 const uint8_t* mask ) {}

void ZNetHandler::onNodeCmdDoorLockCapabilities( const uint8_t nodeId,
                                                 const uint8_t channelId,
                                                 const znet_door_lock_capabilities_t& capabilities ) {}

void ZNetHandler::onNodeCmdDoorLockOperation( const uint8_t nodeId,
                                              const uint8_t channelId,
                                              const DoorLock::Mode mode,
                                              const uint8_t insideHandleModes,
                                              const uint8_t outsideHandleModes,
                                              const uint8_t condition,
                                              SecondsOpt lockTimeout,
                                              const DoorLock::ModeOpt tagretMode,
                                              const SecondsOpt tagretModeTimeOut ) {}

void ZNetHandler::onNodeCmdDoorLockConfiguration(
    const uint8_t nodeId,
    const uint8_t channelId,
    const znet_door_lock_configuration_t& configuration,
    const znet_door_lock_extended_configuration_t* extendedConfiguration ) {}

void ZNetHandler::onNodeCmdNotification( const uint8_t node_id,
                                         const uint8_t channel_id,
                                         const znet_notification_report_t& value,
                                         bool isNotificationRequested ) {}

void ZNetHandler::onNodeCmdNotificationSupported( const uint8_t node_id,
                                                  const uint8_t channel_id,
                                                  const znet_notification_supported_report_t* value ) {}

void ZNetHandler::onNodeCmdNotificationEventSupported( const uint8_t node_id,
                                                       const uint8_t channel_id,
                                                       const uint8_t type,
                                                       const uint8_t* bitmask,
                                                       uint8_t bitmaskSize ) {}

// void ZNetHandler::onNodeCmdDoorLockConfiguration( const uint8_t nodeId, const uint8_t channelId, const uint8_t value
// ) {}

void ZNetHandler::onNodeCmdMultilevelSwitch( const uint8_t nodeId,
                                             const uint8_t channelId,
                                             const uint8_t currentValue,
                                             const utils::optional<uint8_t> targetValue,
                                             SecondsOpt targetDuration ) {}

void ZNetHandler::onNodeCmdMultilevelSwitchSetDone( const uint8_t nodeId, const uint8_t channelId ) {}

void ZNetHandler::onNodeCmdBarrierOperator( const uint8_t nodeId, const uint8_t channelId, const uint8_t value ) {}

void ZNetHandler::onNodeCmdBarrierOperatorSignal( const uint8_t nodeId,
                                                  const uint8_t channelId,
                                                  const uint8_t signalId,
                                                  const uint8_t state ) {}

void ZNetHandler::onNodeCmdBarrierOperatorSupportedSignals( const uint8_t nodeId,
                                                            const uint8_t channelId,
                                                            const uint8_t* data,
                                                            uint8_t len ) {}

void ZNetHandler::onNodeCmdBarrierOperatorSetDone( const uint8_t nodeId, const uint8_t channelId ) {}

void ZNetHandler::onNodeCmdMeter( const znet_node_id_t nodeId,
                                  const znet_node_channel_id_t channelId,
                                  const meter::MeterType type,
                                  const meter::Scale scale,
                                  const int32_t value,
                                  const uint8_t precision ) {}

void ZNetHandler::onNodeCmdMeterSetDone( const uint8_t nodeId, const uint8_t channelId ) {}

void ZNetHandler::onNodeCmdMultichannelEndpoint( const uint8_t nodeId,
                                                 const uint8_t agregated_channel_ids_count,
                                                 const uint8_t channel_ids_count,
                                                 const uint8_t identical,
                                                 const uint8_t dynamic ) {}

void ZNetHandler::onNodeCmdMultichannelCapability( const uint8_t nodeId,
                                                   const uint8_t channel_id,
                                                   const uint8_t dynamic,
                                                   const uint8_t generic,
                                                   const uint8_t specific,
                                                   const uint8_t commands_count,
                                                   const znet_command_class_t* commands ) {}

void ZNetHandler::onApplicationNodeUpdate( const znet_nodeinfo_t* nodeInfo ) {
    // FUNC_LOGE( "It tries to use an empty(base) handler onApplicationNodeUpdate()" );
}

void ZNetHandler::onWakeUpResult( const uint8_t nodeId, const uint32_t interval, const znet_node_id_t targetNodeId ) {}

void ZNetHandler::onWakeupCapabilitiesResult( const uint8_t nodeId,
                                              const uint32_t min_wakeup_interval,
                                              const uint32_t max_wakeup_interval,
                                              const uint32_t default_wakeup_interval,
                                              const uint32_t step_wakeup_interval ) {}

void ZNetHandler::onWakeupNotification( const uint8_t nodeId ) {}

void ZNetHandler::onNodeFailed( const uint8_t nodeId, const int error ) {}

void ZNetHandler::onAssociationSupportedGroupingResult( const uint8_t nodeId,
                                                        const uint8_t channelId,
                                                        const uint8_t count ) {}

void ZNetHandler::onAssociationResult( const uint8_t nodeId,
                                       const uint8_t channelId,
                                       const znet_association_report_t* value ) {}

void ZNetHandler::onAssociationSetResult( const uint8_t nodeId, const uint8_t channelId ) {}

void ZNetHandler::onAssociationRemoveResult( const uint8_t nodeId, const uint8_t channelId ) {}

void ZNetHandler::onAssociationGroupNameResult( const uint8_t nodeId,
                                                const uint8_t channelId,
                                                const znet_association_group_name_report_t* value ) {}

void ZNetHandler::onAssociationGroupInfoResult( const uint8_t nodeId,
                                                const uint8_t channelId,
                                                const znet_association_group_info_report_t* value ) {}

void ZNetHandler::onAssociationGroupCommandListResult( const uint8_t nodeId,
                                                       const uint8_t channelId,
                                                       const znet_association_group_command_list_report_t* value ) {}

void ZNetHandler::onMultichannelAssociationSupportedGroupingResult( const uint8_t nodeId,
                                                                    const uint8_t channelId,
                                                                    const uint8_t count ) {}

void ZNetHandler::onMultichannelAssociationResult( const uint8_t nodeId,
                                                   const uint8_t channelId,
                                                   const znet_multichannel_association_report_t* value ) {}

void ZNetHandler::onMultichannelAssociationSetResult( const uint8_t nodeId, const uint8_t channelId ) {}

void ZNetHandler::onMultichannelAssociationRemoveResult( const uint8_t nodeId, const uint8_t channelId ) {}

void ZNetHandler::onSimpleAvControlSetResult( const uint8_t nodeId,
                                              const uint8_t channelId,
                                              const znet_simple_av_set_report_t* value ) {}

void ZNetHandler::onConfigurationReport( const uint8_t nodeId,
                                         const uint8_t channelId,
                                         const uint16_t configParamNum,
                                         const uint8_t configParamSize,
                                         const uint32_t configParamValue ) {}

void ZNetHandler::onConfigurationRequestStatus( const uint8_t nodeId,
                                                const uint8_t channelId,
                                                const uint8_t configParamNum,
                                                RequestStatus::Enum status ) {}

void ZNetHandler::onThermostatModeResult( uint8_t nodeId,
                                          uint8_t channelId,
                                          const znet_thermostat_mode_report_t& value ) {}

void ZNetHandler::onThermostatModeSupportedResult( const uint8_t nodeId,
                                                   const uint8_t channelId,
                                                   const uint8_t mcount,
                                                   const uint8_t* bit_mask ) {}

void ZNetHandler::onThermostatFanModeResult( const uint8_t nodeId, const uint8_t value ) {}

void ZNetHandler::onThermostatFanModeSupportedResult( const uint8_t nodeId,
                                                      const uint8_t channelId,
                                                      const uint8_t fmcount,
                                                      const uint8_t* bit_mask ) {}

void ZNetHandler::onThermostatFanStateResult( const uint8_t nodeId, const uint8_t value ) {}

void ZNetHandler::onThermostatOperatingStateResult( const uint8_t nodeId, const uint8_t value ) {}

void ZNetHandler::onThermostatOperatingStateLoggingSupportedResult( const uint8_t nodeId,
                                                                    const uint8_t channelId,
                                                                    const uint8_t opscount,
                                                                    const uint8_t* bit_mask ) {}

void ZNetHandler::onThermostatOperatingStateLoggingResult( const uint8_t nodeId,
                                                           const uint8_t rep_to_follows,
                                                           const uint16_t data_count,
                                                           const uint8_t* data ) {}

void ZNetHandler::onThermostatSetpointResult( const uint8_t nodeId,
                                              const uint8_t channelId,
                                              const uint8_t type,
                                              const uint8_t scale,
                                              const uint8_t size,
                                              const uint8_t precision,
                                              const int32_t sp_value,
                                              const bool isSetpointRequested ) {}

void ZNetHandler::onThermostatSetbackResult( const znet_node_id_t nodeId,
                                             const uint8_t channelId,
                                             const thermostat_setback::OverrideType type,
                                             const thermostat_setback::Mode mode,
                                             const thermostat_setback::SetbackValueOpt valueOpt ) {}

void ZNetHandler::onThermostatSetpointSupportedResult( const uint8_t nodeId,
                                                       const uint8_t channelId,
                                                       const uint8_t stcount,
                                                       const uint8_t* bit_mask ) {}

void ZNetHandler::onThermostatSetpointCapabilitiesResult( const uint8_t nodeId,
                                                          const uint8_t channelId,
                                                          const uint8_t type,
                                                          const uint8_t minParams,
                                                          const int32_t minValue,
                                                          const uint8_t maxParams,
                                                          const int32_t maxValue ) {}

void ZNetHandler::onNodeCmdUserCodeReport( const znet_node_id_t /*nodeId*/,
                                           const znet_node_channel_id_t /*channelId*/,
                                           const znet_user_code_report_t* /*userCodeReport*/,
                                           uint8_t /*count*/,
                                           UserCodeNumberOpt /*nextUserCode*/,
                                           UserCode::UserPollStatus /*pollStatus*/ ) {}

void ZNetHandler::onNodeCmdUsersNumberReport( const znet_node_id_t nodeId,
                                              const znet_node_channel_id_t channelId,
                                              const znet_supported_users_t supportedUsers ) {}

void ZNetHandler::onNodeCmdHail( const znet_node_id_t nodeId ) {}

void ZNetHandler::startInclude( const DskDerivedHomeId& dskDerivedHomeId ) {}
void ZNetHandler::stopInclude() {}
void ZNetHandler::startExclude() {}
void ZNetHandler::startExclude( const ::znet::ZNode& ) {}
void ZNetHandler::stopExclude() {}
void ZNetHandler::stopLearn() {}

void ZNetHandler::onZwavePlusInfoGet() {}

void ZNetHandler::onIndicatorIdentify() {}

SecurityMode::Class ZNetHandler::onSecurityGet( znet_node_id_t node_id ) {
    ZNode* node = m_parentMesh.onFindNode( node_id );
    if( node != nullptr ) {
        return node->getSecurity();
    }
    return SecurityMode::Class::None;
}

void ZNetHandler::onTimeRequested( const uint8_t nodeId, const uint8_t channelId, const uint8_t encap ) {
    m_parentMesh.getTimeManager().onTimeRequested( nodeId, channelId, encap );
}

void ZNetHandler::onDateRequested( const uint8_t nodeId, const uint8_t channelId, const uint8_t encap ) {
    m_parentMesh.getTimeManager().onDateRequested( nodeId, channelId, encap );
}

void ZNetHandler::onTimeZoneRequested( const uint8_t nodeId, const uint8_t channelId, const uint8_t encap ) {
    m_parentMesh.getTimeManager().onTimeZoneRequested( nodeId, channelId, encap );
}

void ZNetHandler::onNodeTimeZoneSet( znet_node_id_t nodeId,
                                     znet_node_channel_id_t channelId,
                                     const znet_time_zone_settings_t& timeZoneSettings ) {
    m_parentMesh.getTimeManager().onTimeZoneSet( nodeId, channelId, timeZoneSettings );
}

void ZNetHandler::onNodeTimeZoneReport( znet_node_id_t nodeId,
                                        znet_node_channel_id_t channelId,
                                        const znet_time_zone_settings_t& timeZoneSettings ) {}

void ZNetHandler::onNodeLocalTimeReport( znet_node_id_t nodeId,
                                         znet_node_channel_id_t channelId,
                                         const znet_local_time_t& localTime ) {}

void ZNetHandler::onNodeLocalDateReport( znet_node_id_t nodeId,
                                         znet_node_channel_id_t channelId,
                                         const znet_local_date_t& localDate ) {}

void ZNetHandler::onNodeScheduleLockEntryTimeOffsetReport(
    znet_node_id_t /*node_id*/,
    znet_node_channel_id_t /*channel_id*/,
    const znet_cmd_schedule_entry_lock_time_offset_t& /*value*/ ) {}

void ZNetHandler::onNodeScheduleEntryLockWeekDayReport( znet_node_id_t /*node_id*/,
                                                        znet_node_channel_id_t /*channel_id*/,
                                                        const znet_week_day_schedule_t& /*value*/ ) {}

void ZNetHandler::onNodeScheduleEntryLockYearDayReport( znet_node_id_t /*node_id*/,
                                                        znet_node_channel_id_t /*channel_id*/,
                                                        const znet_year_day_schedule_t& /*value*/ ) {}

void ZNetHandler::onNodeScheduleLockEntrySupportedReport(
    znet_node_id_t /*node_id*/,
    znet_node_channel_id_t /*channel_id*/,
    const znet_cmd_schedule_entry_lock_supported_report_t& /*value*/ ) {}

void ZNetHandler::onNodeScheduleEntryLockDailyRepeatingReport( znet_node_id_t /*node_id*/,
                                                               znet_node_channel_id_t /*channel_id*/,
                                                               const znet_daily_repeating_schedule_t& /*value*/ ) {}

void ZNetHandler::onNodeSoundSwitchTonesNumberReport( znet_node_id_t /*nodeId*/,
                                                      znet_node_channel_id_t /*channelId*/,
                                                      znet_cmd_sound_switch_tones_number_t /*value*/ ) {}

void ZNetHandler::onNodeSoundSwitchToneInfoReport( znet_node_id_t /*nodeId*/,
                                                   znet_node_channel_id_t /*channelId*/,
                                                   const znet_cmd_sound_switch_tone_info_t& /*value*/ ) {}

void ZNetHandler::onNodeSoundSwitchConfigurationReport( znet_node_id_t /*nodeId*/,
                                                        znet_node_channel_id_t /*channelId*/,
                                                        const znet_cmd_sound_switch_configuration_t& /*value*/ ) {}

void ZNetHandler::onNodeSoundSwitchTonePlayReport( znet_node_id_t /*nodeId*/,
                                                   znet_node_channel_id_t /*channelId*/,
                                                   const znet_cmd_sound_switch_tone_play_t& /*value*/ ) {}

void ZNetHandler::onNodeUserCodeCapabilitiesReport(
    znet_node_id_t /*nodeId*/,
    znet_node_channel_id_t /*channelId*/,
    const znet_user_code_capabilities_report_t& /*userCodeCapabilities*/ ) {}

void ZNetHandler::onNodeUserCodesKeypadModeReport( znet_node_id_t /*nodeId*/,
                                                   znet_node_channel_id_t /*channelId*/,
                                                   znet_user_code_keypad_mode_t /*keypadMode*/ ) {}

void ZNetHandler::onNodeExtendedUserCodeReport( znet_node_id_t /*nodeId*/,
                                                znet_node_channel_id_t /*channelId*/,
                                                uint8_t /*userCodesCount*/,
                                                const znet_user_code_report_t* /*userCodes*/ ) {}

void ZNetHandler::onNodeUserCodeChecksumReport( znet_node_id_t /*nodeId*/,
                                                znet_node_channel_id_t /*channelId*/,
                                                znet_user_code_checksum_t /*checksum*/ ) {}

void ZNetHandler::onNodeMasterCodeReport( znet_node_id_t /*nodeId*/,
                                          znet_node_channel_id_t /*channelId*/,
                                          uint8_t /*codeLemgth*/,
                                          const uint8_t* /*masterCode*/ ) {}

void ZNetHandler::onNodeInclusionControllerInitiate( znet_node_id_t /*nodeId*/,
                                                     const znet_node_id_t /*childNodeId*/,
                                                     const znet_inclution_controller_step_id_t /*stepId*/ ) {}

void ZNetHandler::onNodeInclusionControllerComplete( znet_node_id_t /*nodeId*/,
                                                     const znet_inclution_controller_step_id_t /*stepId*/,
                                                     const znet_inclution_controller_status_t /*status*/ ) {}

void ZNetHandler::onNodeFirmwareMdReport( znet_node_id_t /*nodeId*/,
                                          znet_node_channel_id_t /*channelId*/,
                                          const znet_firmware_md_report_v1_t& /*reportV1*/,
                                          const znet_firmware_md_report_v3_t* /*reportV3Opt*/,
                                          const znet_firmware_md_report_v5_t* /*reportV5Opt*/
) {}

void ZNetHandler::onNodeCmdFirmwareUpdateMdRequestReport( znet_node_id_t /*nodeId*/,
                                                          znet_node_channel_id_t /*channelId*/,
                                                          FirmwareUpdate::UpdateMdRequestReportStatus /*status*/ ) {}

void ZNetHandler::onNodeCmdFirmwareUpdateMdRequested( znet_node_id_t nodeId,
                                                      znet_node_channel_id_t channelId,
                                                      uint8_t /*requestedReportsCount*/,
                                                      uint16_t reportNumber ) {
    auto node = m_parentMesh.onFindNode( nodeId );
    auto cc = node ? node->getCommandClass<ZCc0x7AFirmwareUpdate>( channelId ) : nullptr;

    if( cc ) {
        cc->onDataRequestedUnexpectedly( reportNumber );
    }
}

void ZNetHandler::onNodeCmdFirmwareUpdateMdStatusReport( znet_node_id_t /*nodeId*/,
                                                         znet_node_channel_id_t /*channelId*/,
                                                         FirmwareUpdate::DataSendingStatus /*status*/,
                                                         uint16_t /*waitingTime*/ ) {}

void ZNetHandler::onNodeFirmwareUpdateMdActivationStatusReport( znet_node_id_t /*nodeId*/,
                                                                znet_node_channel_id_t /*channelId*/,
                                                                uint8_t /*target*/,
                                                                const znet_firmware_info_t& /*firmware*/,
                                                                FirmwareUpdate::ActivationStatus /*status*/,
                                                                Uint8Opt /*hardwareId*/ ) {}

void ZNetHandler::onNodeCmdAntiTheftReport( znet_node_id_t nodeId,
                                            znet_node_channel_id_t channelId,
                                            const znet_cmd_anti_theft_report_t& report ) {}

void ZNetHandler::onNodeCmdAntiTheftUnlockStateReport( znet_node_id_t nodeId,
                                                       znet_node_channel_id_t channelId,
                                                       const znet_cmd_anti_theft_unlock_state_report_t& report ) {}

void ZNetHandler::onNodeProvisioningSet( znet_node_id_t nodeId,
                                         znet_node_channel_id_t channelId,
                                         const znet_node_provisioning_set_t* value ) {
    m_parentMesh.getNodeProvisioningManager().onNodeProvisioningSet( nodeId, channelId, value );
}

void ZNetHandler::onNodeProvisioningDelete( znet_node_id_t nodeId,
                                            znet_node_channel_id_t channelId,
                                            const znet_node_provisioning_delete_t* value ) {
    m_parentMesh.getNodeProvisioningManager().onNodeProvisioningDelete( nodeId, channelId, value );
}

void ZNetHandler::onNodeProvisioningGet( znet_node_id_t nodeId,
                                         znet_node_channel_id_t channelId,
                                         uint8_t sequenceNumber,
                                         const uint8_t dskKey[16] ) {
    m_parentMesh.getNodeProvisioningManager().onNodeProvisioningGet( nodeId, channelId, sequenceNumber, dskKey );
}

void ZNetHandler::onNodeProvisioningListIterationGet( znet_node_id_t nodeId,
                                                      znet_node_channel_id_t channelId,
                                                      uint8_t sequenceNumber,
                                                      uint8_t remainingCounter ) {
    m_parentMesh.getNodeProvisioningManager().onNodeProvisioningListIterationGet(
        nodeId, channelId, sequenceNumber, remainingCounter );
}

bool ZNetHandler::canHandleNode( znet_node_id_t /*nodeId*/ ) const {
    return true;
}

void ZNetHandler::updateReachableState( ZNode& node, bool forceNotify ) {
    m_parentMesh.setReachableState( node.getId(), true, forceNotify );
}

}  // namespace znet
