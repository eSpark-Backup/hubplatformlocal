#include "znet/cc_0x63_user_code.hpp"

#include "znet/znet_node.hpp"
#include "znet/znet_requests_manager.hpp"
#include "znet/znet_utils.hpp"

#include <cassert>

extern "C" {
#include "znet_log.h"
}

namespace znet {

namespace {

constexpr size_t MIN_USER_CODE_SIZE = 4;
constexpr size_t MAX_USER_CODE_SIZE = 10;

template <typename Capabilities, typename ExtraSettings, typename Handler>
void forEachFeature( Capabilities& capabilities, ExtraSettings& extraSettings, Handler&& handler ) {
    handler( capabilities.isCheckSumSupported, 0 );
    handler( capabilities.isMasterCodeSupported, 1 );
    handler( capabilities.isMasterCodeDeactivationSupported, 2 );
    handler( capabilities.isMultipleUserCodeReportSupported, 3 );
    handler( capabilities.isMultipleUserCodeSetSupported, 4 );

    handler( extraSettings.isExtendedUserCodesRequestRestricted, 5 );
}

template <typename VectorType, typename ReportedCapability>
void fillCapabilityVector( std::vector<VectorType>& dest, const ReportedCapability& capability ) {
    dest.clear();
    dest.reserve( utils::getBitsCount( capability.bitmask, capability.bitmask_length ) );
    utils::forEachBit( capability.bitmask, capability.bitmask_length, [&dest]( uint8_t index ) {
        dest.push_back( static_cast<VectorType>( index ) );
    } );
}

}  // namespace

ZCc0x63UserCode::UserIdsCache::UserIdsCache( ZCc0x63UserCode& cc )
: m_userCodeCC( cc ) {}

void ZCc0x63UserCode::UserIdsCache::onUserCodeUpdated( znet_supported_users_t userId, UserCode::UserIDStatus status ) {
    const bool userCodeExists = status == UserCode::UserIDStatus::Enabled ||
                                status == UserCode::UserIDStatus::Messaging ||
                                status == UserCode::UserIDStatus::PassageMode;
    if( userId == 0 ) {
        if( userCodeExists ) {
            for( userId = 1; userId <= m_userCodeCC.getSupportedUsersCount(); ++userId )
                m_userIds.insert( userId );
        }
        else {
            m_userIds.clear();
        }
    }
    else if( userCodeExists ) {
        m_userIds.insert( userId );
    }
    else {
        m_userIds.erase( userId );
    }
}

const ZCc0x63UserCode::UserIds& ZCc0x63UserCode::UserIdsCache::getUserIds() const {
    return m_userIds;
}

ZCc0x63UserCode::ZCc0x63UserCode( ZChannel& parentChannel, const znet_command_class_version_t version )
: ZClass( parentChannel, version )
, m_userIdsCache( *this ) {}

bool ZCc0x63UserCode::interview( InterviewCallback callback, WeakJobContext context ) {
    auto callbackPtr = callback ? std::make_shared<InterviewCallback>( std::move( callback ) ) : nullptr;

    return getParentNode().getRequestsManager().doRequest(
        UserCodesNumberRequest( getNodeId(), getChannelId() ),
        context,
        [=]( znet_supported_users_t usersCount ) {
            m_supportedUsersCount = usersCount;

            if( getVersion() > 1 ) {
                auto onFail = [=]( RequestFailType type ) {
                    ZNET_LOGE( "User codes number request failed! Interview of user code cc cannot be finished!\n" );
                    onRequestsFinished( callbackPtr, type );
                };

                const auto capabilitiesResult = getParentNode().getRequestsManager().doRequest(
                    UserCodeCapabilitiesRequest( getNodeId(), getChannelId() ),
                    context,
                    [=]( const znet_user_code_capabilities_report_t& capabilitiesReport ) {
                        setCapabilitiesFromReport( capabilitiesReport );
                        setInterviewDone( true );
                        onRequestsFinished( callbackPtr, {} );
                    },
                    onFail );

                if( !capabilitiesResult ) {
                    onFail( RequestFailType::SendingError );
                }
            }
            else {
                onRequestsFinished( callbackPtr, {} );

                setInterviewDone( true );
            }
        },
        [=]( RequestFailType failType ) {
            onRequestsFinished( callbackPtr, failType );

            ZNET_LOGE( "User codes number request failed! Interview of user code cc cannot be finished!\n" );
        } );
}

void ZCc0x63UserCode::setUserCode( znet_supported_users_t userId, uint8_t userIdStatus, const std::string& userCode ) {
    ZNET_LOGI( "%s: user id: %u, status: %u, code: %s\n", __func__, userId, userIdStatus, userCode.c_str() );

    if( static_cast<UserCode::UserIDStatus>( userIdStatus ) == UserCode::UserIDStatus::NotAvailable ) {
        assert( false );
        ZNET_LOGI( "invalid userIdStatus(%d)\n", userIdStatus );
        return;
    }

    static const uint8_t EMPTY_USER_CODE[MIN_USER_CODE_SIZE] = { 0 };

    const uint8_t* userCodeData = EMPTY_USER_CODE;
    uint8_t userCodeDataLength = MIN_USER_CODE_SIZE;

    if( static_cast<UserCode::UserIDStatus>( userIdStatus ) != UserCode::UserIDStatus::Available ) {
        if( userCode.size() < MIN_USER_CODE_SIZE ) {
            ZNET_LOGW( "userCode is too short\n" );
            return;
        }
        else if( userCode.size() > MAX_USER_CODE_SIZE ) {
            ZNET_LOGW( "userCode is too long\n" );
            return;
        }

        userCodeData = reinterpret_cast<const uint8_t*>( userCode.c_str() );
        userCodeDataLength = static_cast<std::uint8_t>( userCode.size() );
    }
    else if( getVersion() > 1 ) {
        userCodeDataLength = 0;
    }

    if( getVersion() == 1 ) {
        const auto result = znet_node_cmd_user_code_set( getNodeId(),
                                                         getChannelId(),
                                                         static_cast<uint8_t>( userId ),
                                                         userIdStatus,
                                                         userCodeDataLength,
                                                         userCodeData );
        if( result != ZNET_ERR_OK ) {
            ZNET_LOGE(
                "znet_node_cmd_user_code_set failed for node %u, user_id %u, error %u\n", getNodeId(), userId, result );
        }
    }
    else {
        const znet_user_code_report_t userCode{ .user_id = userId,
                                                .user_id_status = userIdStatus,
                                                .user_code_size = userCodeDataLength,
                                                .user_code = userCodeData };

        const auto result = znet_node_cmd_extended_user_code_set( getNodeId(), getChannelId(), 1, &userCode );
        if( result != ZNET_ERR_OK ) {
            ZNET_LOGE( "znet_node_cmd_user_code_set failed for node %u, error %u\n", getNodeId(), result );
        }
    }
}

void ZCc0x63UserCode::setMasterCode( const std::string& masterCode ) {
    if( getVersion() < 2 ) {
        ZNET_LOGW( "Cannot set master code for first version of user code CC" );
        return;
    }

    if( !m_capabilities.isMasterCodeSupported ) {
        ZNET_LOGW( "Master code is not supported for this device" );
        return;
    }

    if( masterCode.empty() ) {
        if( !m_capabilities.isMasterCodeDeactivationSupported ) {
            ZNET_LOGW( "Master code deactivation not supported" );
            return;
        }
    }
    else if( masterCode.size() < MIN_USER_CODE_SIZE ) {
        ZNET_LOGW( "master code is too short\n" );
        return;
    }
    else if( masterCode.size() > MAX_USER_CODE_SIZE ) {
        ZNET_LOGI( "master code is too long\n" );
        return;
    }

    const auto result = znet_node_cmd_master_code_set( getNodeId(),
                                                       getChannelId(),
                                                       static_cast<uint8_t>( masterCode.size() ),
                                                       reinterpret_cast<const uint8_t*>( masterCode.data() ) );
    if( result != ZNET_ERR_OK ) {
        ZNET_LOGE( "znet_node_cmd_master_code_set failed for node %u, error %u\n", getNodeId(), result );
    }
}

void ZCc0x63UserCode::setKeypadMode( UserCode::KeypadMode keypadMode ) {
    const auto result =
        znet_node_cmd_user_code_keypad_mode_set( getNodeId(), getChannelId(), static_cast<uint8_t>( keypadMode ) );

    if( result != ZNET_ERR_OK ) {
        ZNET_LOGE( "znet_node_cmd_user_code_keypad_mode_set failed for node %u, error %u\n", getNodeId(), result );
    }
}

void ZCc0x63UserCode::setExtendedUserCodesRequestRestricted( bool restricted ) {
    m_extraSettings.isExtendedUserCodesRequestRestricted = restricted;
}

void ZCc0x63UserCode::setUnsetPinCodesCountToStop( size_t count ) {
    m_extraSettings.unsetPinCodesCountToStop = count;
}

bool ZCc0x63UserCode::canRequestExtendedUserCodes() const {
    return getVersion() > 1 && !m_extraSettings.isExtendedUserCodesRequestRestricted;
}

bool ZCc0x63UserCode::requestMasterCode( std::function<void( const char*, uint8_t )> onSuccess,
                                         std::function<void( RequestFailType )> onFail,
                                         WeakJobContext context ) const {
    return getParentNode().getRequestsManager().doRequest( MasterCodeRequest( getNodeId(), getChannelId() ),
                                                           context,
                                                           // TODO: use moving to lambda on c++14
                                                           [onSuccess]( uint8_t count, const uint8_t* data ) {
                                                               if( onSuccess ) {
                                                                   onSuccess( reinterpret_cast<const char*>( data ),
                                                                              count );
                                                               }
                                                           },
                                                           std::move( onFail ) );
}

bool ZCc0x63UserCode::requestChecksum( std::function<void( znet_user_code_checksum_t )> onSuccess,
                                       std::function<void( RequestFailType )> onFail,
                                       WeakJobContext context ) const {
    return getParentNode().getRequestsManager().doRequest(
        UserCodeChecksumRequest( getNodeId(), getChannelId() ), context, std::move( onSuccess ), std::move( onFail ) );
}

bool ZCc0x63UserCode::requestKeypadMode( std::function<void( znet_user_code_keypad_mode_t )> onSuccess,
                                         std::function<void( RequestFailType )> onFail,
                                         WeakJobContext context ) const {
    return getParentNode().getRequestsManager().doRequest( UserCodesKeypadModeRequest( getNodeId(), getChannelId() ),
                                                           context,
                                                           std::move( onSuccess ),
                                                           std::move( onFail ) );
}

bool ZCc0x63UserCode::requestValue( const RequestValue& request ) {
    if( const auto userIdRequest = request.toRequestValueUserCode() ) {
        if( canRequestExtendedUserCodes() ) {
            return requestValueImpl(
                *this,
                ExtendedCodesRequest(
                    getNodeId(), getChannelId(), userIdRequest->userId, userIdRequest->multipleRequest ),
                request );
        }
        else {
            return requestValueImpl(
                *this, UserCodesRequest( getNodeId(), getChannelId(), userIdRequest->userId ), request );
        }
    }

    const auto callbackPtr =
        request.resultCallback ? std::make_shared<RequestValue::Callback>( request.resultCallback ) : nullptr;
    const auto context = request.jobContext;

    auto handleSendingResult = [=]( bool result ) {
        if( !result ) {
            onRequestsFinished( callbackPtr, RequestFailType::SendingError );
        }
    };

    auto doRequestAllUserCodes = [=]( bool isPostponed ) {
        if( getSupportedUsersCount() ) {
            return requestAllUserCodesImpl( callbackPtr, context );
        }

        if( isPostponed ) {
            onRequestsFinished( callbackPtr, {} );
        }

        return isPostponed;
    };

    auto doRequestChecksum = [=]( bool isPostponed ) {
        if( m_capabilities.isCheckSumSupported ) {
            auto onResult = std::bind( handleSendingResult, std::bind( doRequestAllUserCodes, true ) );
            return requestChecksum( onResult, onResult, context ) || doRequestAllUserCodes( isPostponed );
        }
        else {
            return doRequestAllUserCodes( isPostponed );
        }
    };

    auto doRequestKeypadMode = [=]( bool isPostponed ) {
        if( m_capabilities.supportedKeypadModes.empty() ) {
            return doRequestChecksum( isPostponed );
        }
        else {
            auto onResult = std::bind( handleSendingResult, std::bind( doRequestChecksum, true ) );
            return requestKeypadMode( onResult, onResult, context ) || doRequestChecksum( isPostponed );
        }
    };

    auto doRequestMasterCode = [=]( bool isPostponed ) {
        if( m_capabilities.isMasterCodeSupported ) {
            auto onResult = std::bind( handleSendingResult, std::bind( doRequestKeypadMode, true ) );
            return requestMasterCode( onResult, onResult, context ) || doRequestKeypadMode( isPostponed );
        }
        else {
            return doRequestKeypadMode( isPostponed );
        }
    };

    return doRequestMasterCode( false );
}

void ZCc0x63UserCode::setSupportedUsersCount( znet_supported_users_t supportedUsersCount ) {
    m_supportedUsersCount = supportedUsersCount;
}

znet_supported_users_t ZCc0x63UserCode::getSupportedUsersCount() const {
    return m_supportedUsersCount;
}

bool ZCc0x63UserCode::requestAllUserCodes( RequestValue::Callback cb, WeakJobContext jobContext ) {
    return requestAllUserCodesImpl( cb ? std::make_shared<RequestValue::Callback>( std::move( cb ) ) : nullptr,
                                    jobContext );
}

bool ZCc0x63UserCode::requestAllUserCodesImpl( RequestValue::CallbackPtr callbackPtr, WeakJobContext jobContext ) {
    if( getSupportedUsersCount() == 0 ) {
        return false;
    }

    m_unsetPinCodesCount = 0;
    requestNextUserCode( std::move( callbackPtr ), jobContext, ++m_pollingIndex );
    return true;
}

void ZCc0x63UserCode::requestNextUserCode( RequestValue::CallbackPtr callbackPtr,
                                           WeakJobContext jobContext,
                                           size_t pollingIndex,
                                           znet_supported_users_t userId ) {
    assert( userId );

    if( userId > getSupportedUsersCount() || pollingIndex != m_pollingIndex ) {
        onRequestsFinished( callbackPtr, {} );
        return;
    }

    if( m_extraSettings.unsetPinCodesCountToStop != ExtraSettings::dontStopPinCodesScanning &&
        m_unsetPinCodesCount >= m_extraSettings.unsetPinCodesCountToStop ) {
        userId = getSupportedUsersCount();
    }

    auto onResult = [=]( znet_supported_users_t nextUserId, UserCode::UserPollStatus* status ) {
        if( status ) {
            *status = nextUserId > getSupportedUsersCount() ? UserCode::UserPollStatus::Finished :
                                                              UserCode::UserPollStatus::Polling;
        }
        requestNextUserCode( callbackPtr, jobContext, pollingIndex, nextUserId );
    };

    if( canRequestExtendedUserCodes() ) {
        getParentNode().getRequestsManager().doRequest(
            ExtendedCodesRequest( getNodeId(), getChannelId(), userId, true ),
            jobContext,
            [=]( uint8_t count,
                 const znet_user_code_report_t* userCodes,
                 znet_supported_users_t nextUserId,
                 UserCode::UserPollStatus* status ) {
                if( nextUserId == 0 ) {
                    nextUserId = getSupportedUsersCount() + 1;
                }
                else {
                    assert( count >= 1 );
                    for( size_t i = 0; i < count; ++i ) {
                        nextUserId =
                            std::max( nextUserId, static_cast<znet_supported_users_t>( userCodes[i].user_id + 1 ) );
                    }
                }

                onResult( nextUserId, status );
            },
            std::bind( onRequestsFinished, callbackPtr, std::placeholders::_1 ),
            std::max( DefaultPostponedRequestDelay, getParentNode().getRequestDelay() ) );
    }
    else {
        getParentNode().getRequestsManager().doRequest(
            UserCodesRequest( getNodeId(), getChannelId(), userId ),
            jobContext,
            [=]( const znet_user_code_report_t& report, UserCode::UserPollStatus* status ) {
                if( static_cast<UserCode::UserIDStatus>( report.user_id_status ) != UserCode::UserIDStatus::Enabled ) {
                    ++m_unsetPinCodesCount;
                }
                else {
                    m_unsetPinCodesCount = 0;
                }

                onResult( userId + 1, status );
            },
            std::bind( onRequestsFinished, callbackPtr, std::placeholders::_1 ),
            std::max( DefaultPostponedRequestDelay, getParentNode().getRequestDelay() ) );
    }
}

ZCc0x63UserCode::UserIdsCache& ZCc0x63UserCode::getUserIdsCache() {
    return m_userIdsCache;
}

const ZCc0x63UserCode::Capabilities* ZCc0x63UserCode::getCapabilities() const {
    return getVersion() > 1 ? &m_capabilities : nullptr;
}

bool ZCc0x63UserCode::storeData( IFile& file ) const {
    bool result = file.write( &m_supportedUsersCount, sizeof( m_supportedUsersCount ) );
    uint8_t supportedFeatures = 0;
    forEachFeature( m_capabilities, m_extraSettings, [&supportedFeatures]( bool value, uint8_t index ) {
        if( value ) {
            supportedFeatures |= ( 1 << index );
        }
    } );

    result &= file.write( &supportedFeatures, sizeof( supportedFeatures ) );

    result &= utils::writeVectorToFile( file, m_capabilities.supportedKeys );
    result &= utils::writeVectorToFile( file, m_capabilities.supportedKeypadModes );
    result &= utils::writeVectorToFile( file, m_capabilities.supportedUserIdStatuses );

    result &=
        file.write( &m_extraSettings.unsetPinCodesCountToStop, sizeof( m_extraSettings.unsetPinCodesCountToStop ) );

    return result;
}

bool ZCc0x63UserCode::restoreData( IFile& file, int version ) {
    bool result = false;
    if( version >= majorFormatVersion_v4 ) {
        result = file.read( &m_supportedUsersCount, sizeof( m_supportedUsersCount ) );

        uint8_t supportedFeatures = 0;
        result &= file.read( &supportedFeatures, sizeof( supportedFeatures ) );
        forEachFeature( m_capabilities, m_extraSettings, [&supportedFeatures]( bool& value, uint8_t index ) {
            value = ( supportedFeatures & ( 1 << index ) ) != 0;
        } );

        result &= utils::readVectorFromFile( file, m_capabilities.supportedKeys );
        result &= utils::readVectorFromFile( file, m_capabilities.supportedKeypadModes );
        result &= utils::readVectorFromFile( file, m_capabilities.supportedUserIdStatuses );

        if( version >= majorFormatVersion_v26 ) {
            result = file.read( &m_extraSettings.unsetPinCodesCountToStop,
                                sizeof( m_extraSettings.unsetPinCodesCountToStop ) );
        }
    }
    else {
        uint8_t supportedUsersCount = 0;
        result = file.read( &supportedUsersCount, sizeof( supportedUsersCount ) );
        m_supportedUsersCount = static_cast<znet_supported_users_t>( supportedUsersCount );
    }
    return result;
}

void ZCc0x63UserCode::setCapabilitiesFromReport( const znet_user_code_capabilities_report_t& report ) {
    m_capabilities.isCheckSumSupported = report.keypad_modes_capability.UCC_support;
    m_capabilities.isMasterCodeSupported = report.user_id_status_capability.MC_support;
    m_capabilities.isMasterCodeDeactivationSupported = report.user_id_status_capability.MCD_support;
    m_capabilities.isMultipleUserCodeSetSupported = report.keypad_modes_capability.MUCS_support;
    m_capabilities.isMultipleUserCodeReportSupported = report.keypad_modes_capability.MUCR_support;

    fillCapabilityVector( m_capabilities.supportedKeys, report.keys_capability );
    fillCapabilityVector( m_capabilities.supportedKeypadModes, report.keypad_modes_capability );
    fillCapabilityVector( m_capabilities.supportedUserIdStatuses, report.user_id_status_capability );

    auto isNotPrintable = []( unsigned char c ) { return !std::isprint( c ); };
    m_capabilities.supportedKeys.erase(
        std::remove_if( m_capabilities.supportedKeys.begin(), m_capabilities.supportedKeys.end(), isNotPrintable ),
        m_capabilities.supportedKeys.end() );
}

}  // namespace znet
