#include <znet/znet_node.hpp>
#include <znet/znet_command_class_utils.hpp>
#include <znet/znet_reachable_check_timeout_settings.hpp>

#include <znet/cc_0x86_version.hpp>
#include <znet/cc_0x7a_firmware_update.hpp>
#include <znet/cc_0x84_wakeup.hpp>

extern "C" {
#include <znet_log.h>
}

#include <cassert>
#include <algorithm>

namespace znet {

const int ZNode::WaitingDelayMs = 100;
const int ZNode::WaitingDelayS0Ms = 300;

namespace {

static const int majorFormatVersion = majorFormatVersion_v26;

}  // namespace

ZNode::ZNode( RequestsManager* requestsManager,
              IAssociationsConfigurator* associationsConfigurator,
              ReachableCheckTimeoutSettings* reachableCheckTimeoutSettings )
: m_associationsConfigurator( associationsConfigurator )
, m_id{}
, m_manufacturerId{}
, m_productType{}
, m_productId{}
, m_genericType{}
, m_basicType{}
, m_specificType{}
, m_security( SecurityMode::Class::None )
, m_securityClassStarted( SecurityMode::Class::None )
, m_zwavePlusIconType( 0 )
, m_flags( 0 )
, m_capability( 0 )
, m_isSmartStart{ false }
, m_aggregatedChannelsCount( 0 )
, m_lastDataUpdateTime( std::chrono::steady_clock::now() )
, m_timeOffset{ 0 }
, m_requestDelay{ 0 }
, m_requestsManager( requestsManager )
, m_reachableCheckTimeoutSettings( reachableCheckTimeoutSettings )
, m_isReachable( ReachableState::Unknown )
, m_isWokenUp( false )
, m_roleType( ZNET_CMD_BASIC_VALUE_UNKNOWN )
, m_needForcedRequests( false ) {
    // Every Node has Channel_0
    m_nodeChannels.reserve( 1 );
    m_nodeChannels.emplace_back( static_cast<const uint8_t>( ZNET_CHANNEL_ID_ROOT ), *this );
}

void ZNode::init( znet_node_id_t nodeId, SecurityMode::Class security ) {
    m_id = nodeId;
    m_security = security;
    m_jobs.setID( nodeId );
}

void ZNode::reset() {
    m_id = {};
    m_manufacturerId = {};
    m_productType = {};
    m_productId = {};
    m_genericType = {};
    m_basicType = {};
    m_specificType = {};
    m_security = SecurityMode::Class::None;
    m_securityClassStarted = SecurityMode::Class::None;
    m_zwavePlusIconType = 0;
    m_flags = 0;
    m_capability = 0;
    m_isSmartStart = false;

    m_activeWakeUpInterval.reset();
    m_deviceSpecificWakeUpInterval.reset();

    m_nodeChannels.clear();
    m_aggregatedChannelsCount = 0;

    m_lastDataUpdateTime = std::chrono::steady_clock::now();

    m_timeOffset = std::chrono::minutes( 0 );
    m_requestDelay = std::chrono::milliseconds( 0 );

    m_isReachable = ReachableState::Unknown;
    m_isWokenUp = false;
    m_roleType = ZNET_CMD_BASIC_VALUE_UNKNOWN;
    m_needForcedRequests = false;
    m_isRediscover = false;
    m_isRediscoverRequired = false;
    m_needExtendedRediscover = false;
}

znet_node_id_t ZNode::getId() const {
    return m_id;
}

void ZNode::onRediscoverStarted() {
    assert( !m_isRediscover );
    m_isRediscover = true;
    m_isRediscoverRequired = false;
    m_nodeChannels.clear();
    m_nodeChannels.emplace_back( static_cast<const uint8_t>( ZNET_CHANNEL_ID_ROOT ), *this );
}

void ZNode::onRediscoverFinished() {
    assert( m_isRediscover );
    m_isRediscover = false;
}

bool ZNode::isRediscoverStarted() const {
    return m_isRediscover;
}

bool ZNode::isRediscoverRequired() const {
    return m_isRediscoverRequired;
}

void ZNode::setRediscoverRequired( bool value ) {
    m_isRediscoverRequired = value;
}

bool ZNode::needExtendedRediscover() const {
    return m_needExtendedRediscover;
}

void ZNode::setNeedExtendedRediscover( bool value ) {
    m_needExtendedRediscover = value;
}

ZNode::ClassPtr ZNode::addCommandClass( const znet_node_channel_id_t channelId,
                                        const znet_command_class_t ccid,
                                        const znet_command_class_version_t version,
                                        bool supported ) {
    if( channelId < m_nodeChannels.size() ) {
        if( getCommandClass( channelId, ccid ) ) {
            return {};
        }

        ZChannel& channel = m_nodeChannels[channelId];

        if( supported ) {
            if( auto cc = createCommandClass( ccid, channel, version ) ) {
                return channel.addCommandClass( std::move( cc ) );
            }
        }

        channel.addUnsupportedCommandClass( ccid );
    }

    return {};
}

ZNode::ClassPtr ZNode::getCommandClass( const znet_node_channel_id_t channelId,
                                        const znet_command_class_t ccid ) const {
    if( channelId < m_nodeChannels.size() ) {
        return m_nodeChannels[channelId].getCommandClass( ccid );
    }

    return {};
}

ZNode::ClassPtr ZNode::getCommandClassByIndex( const znet_node_channel_id_t channelId, const uint8_t index ) const {
    if( channelId < m_nodeChannels.size() ) {
        return m_nodeChannels[channelId].getCommandClassByIndex( index );
    }
    return {};
}

uint8_t ZNode::getCommandClassCount( const znet_node_channel_id_t channelId ) const {
    if( channelId < m_nodeChannels.size() ) {
        return m_nodeChannels[channelId].getCommandClassCount();
    }
    return 0;
}

uint8_t ZNode::getUnsupportedCommandClassId( znet_node_channel_id_t channelId, uint8_t idx ) const {
    if( channelId < m_nodeChannels.size() ) {
        return m_nodeChannels[channelId].getUnsupportedCommandClassId( idx );
    }
    return 0;
}

uint8_t ZNode::getUnsupportedCommandClassCount( znet_node_channel_id_t channelId ) const {
    if( channelId < m_nodeChannels.size() ) {
        return m_nodeChannels[channelId].getUnsupportedCommandClassCount();
    }
    return 0;
}

void ZNode::createAdditionalChannels( uint8_t channelsCount ) {
    assert( m_nodeChannels.size() == 1 );
    assert( channelsCount < ZNET_CHANNEL_ID_MAX );

    m_nodeChannels.reserve( channelsCount + 1 );
    for( uint8_t i = 0; i < channelsCount; ++i ) {
        m_nodeChannels.emplace_back( static_cast<const uint8_t>( ZNET_CHANNEL_ID_ROOT + 1 + i ), *this );
    }
}

void ZNode::setAggregatedChannelsCount( uint8_t channelsCount ) {
    m_aggregatedChannelsCount = channelsCount;
}

uint8_t ZNode::getChannelsCount() const {
    return static_cast<uint8_t>( m_nodeChannels.size() );
}

uint8_t ZNode::getAggregatedChannelsCount() const {
    return m_aggregatedChannelsCount;
}

uint8_t ZNode::getAllChannelsCount() const {
    return getChannelsCount() + getAggregatedChannelsCount();
}

void ZNode::setManufacturerSpecific( uint16_t manufacturerId, uint16_t productId, uint16_t productType ) {
    m_manufacturerId = manufacturerId;
    m_productType = productType;
    m_productId = productId;
}

void ZNode::setNodeInfoExtTypes( uint8_t genericType, uint8_t basicType, uint8_t specificType ) {
    m_genericType = genericType;
    m_basicType = basicType;
    m_specificType = specificType;
}

void ZNode::setSecurity( SecurityMode::Class security ) {
    m_security = security;
}

SecurityMode::Class ZNode::getSecurity() const {
    return m_security;
}

void ZNode::setSecurityClassStarted( SecurityMode::Class security ) {
    m_securityClassStarted = security;
}

SecurityMode::Class ZNode::getSecurityClassStarted() const {
    return m_securityClassStarted;
}

void ZNode::setZwavePlusIconType( uint16_t type ) {
    m_zwavePlusIconType = type;
}

uint16_t ZNode::getZwavePlusIconType() const {
    return m_zwavePlusIconType;
}

uint16_t ZNode::getManufacturerId() const {
    return m_manufacturerId;
}

uint16_t ZNode::getProductType() const {
    return m_productType;
}

uint16_t ZNode::getProductId() const {
    return m_productId;
}

uint8_t ZNode::getGenericType() const {
    return m_genericType;
}

uint8_t ZNode::getBasicType() const {
    return m_basicType;
}

uint8_t ZNode::getSpecificType() const {
    return m_specificType;
}

bool ZNode::isSmartStart() const {
    return m_isSmartStart;
}

void ZNode::setIsSmartStart( bool isSmartStart ) {
    m_isSmartStart = isSmartStart;
}

void ZNode::setCapability( uint8_t capability ) {
    m_capability = capability;
}

uint16_t ZNode::getCommandSpan() const {
    if( m_capability & ZNET_NODEINFO_SECURITY_FLIRS_1000 ) {
        return 1000;
    }
    else if( m_capability & ZNET_NODEINFO_SECURITY_FLIRS_250 ) {
        return 250;
    }
    return 0;
}

void ZNode::loop() {}

std::chrono::steady_clock::time_point ZNode::getLastUpdateTime() const {
    return m_lastDataUpdateTime;
}

ReachableState ZNode::isReachable() const {
    return m_isReachable;
}

void ZNode::setReachable( ReachableState reachable ) {
    assert( reachable != ReachableState::Unknown );

    m_isReachable = reachable;

    m_lastDataUpdateTime = std::chrono::steady_clock::now();
}

bool ZNode::isBatteryPowered() const {
    return getCommandClass( ZNET_CHANNEL_ID_ROOT, ZNET_COMMAND_CLASS_BATTERY ) != nullptr;
}

bool ZNode::isWakeup( bool forced ) const {
    return getCommandClass( ZNET_CHANNEL_ID_ROOT, ZNET_COMMAND_CLASS_WAKE_UP ) || ( forced && m_activeWakeUpInterval );
}

void ZNode::setIsWokenUp( bool isWokenUp ) {
    m_isWokenUp = isWokenUp;
}

bool ZNode::isWokenUp() const {
    return m_isWokenUp;
}

void ZNode::setRoleType( uint8_t roleType ) {
    m_roleType = roleType;
}

uint8_t ZNode::getRoleType() const {
    return m_roleType;
}

void ZNode::setForcedRequests( bool hasForcedRequests ) {
    m_needForcedRequests = hasForcedRequests;
}

bool ZNode::hasForcedRequests() const {
    return m_needForcedRequests;
}

void ZNode::setWakeUpInterval( uint32_t interval ) {
    m_activeWakeUpInterval = interval;
}

void ZNode::setDeviceSpecificWakeUpInterval( uint32_t interval ) {
    m_deviceSpecificWakeUpInterval = interval;
}

void ZNode::updateWakeUpInterval() {
    utils::optional<uint32_t> interval = m_deviceSpecificWakeUpInterval;
    auto cc = getCommandClass<ZCc0x84WakeUp>( ZNET_CHANNEL_ID_ROOT );
    if( interval.has_value() || cc ) {
        interval = ( m_reachableCheckTimeoutSettings->useFixedWakeUpInterval() || !interval.has_value() ) ?
                       m_reachableCheckTimeoutSettings->getWakeUpInterval() :
                       interval;
    }

    if( interval ) {
        if( cc ) {
            interval = cc->normalizeWakeUpInterval( *interval );
            if( getWakeUpInterval() != interval ) {
                cc->setWakeUpInterval( *interval );
                interval = cc->getInterval();
            }
        }

        setWakeUpInterval( *interval );
    }
}

utils::optional<uint32_t> ZNode::getWakeUpInterval() const {
    if( m_activeWakeUpInterval ) {
        return m_activeWakeUpInterval;
    }

    if( auto cc = getCommandClass<ZCc0x84WakeUp>( ZNET_CHANNEL_ID_ROOT ) ) {
        if( auto interval = cc->getInterval() ) {
            return { interval };
        }
    }

    return {};
}

bool ZNode::storeData( IFile& file ) const {
    auto result = true;

    auto storeSecurityMode = [&file]( SecurityMode::Class mode ) {
        const auto value = SecurityMode::toByte( mode );

        return file.write( &value, sizeof( value ) );
    };

    result = result && file.write( &majorFormatVersion, sizeof( majorFormatVersion ) );

    result = result && file.write( &m_id, sizeof( m_id ) );
    result = result && file.write( &m_manufacturerId, sizeof( m_manufacturerId ) );
    result = result && file.write( &m_productId, sizeof( m_productId ) );
    result = result && file.write( &m_productType, sizeof( m_productType ) );
    result = result && file.write( &m_genericType, sizeof( m_genericType ) );
    result = result && file.write( &m_basicType, sizeof( m_basicType ) );
    result = result && file.write( &m_specificType, sizeof( m_specificType ) );
    result = result && storeSecurityMode( m_security );
    result = result && storeSecurityMode( m_securityClassStarted );
    result = result && file.write( &m_zwavePlusIconType, sizeof( m_zwavePlusIconType ) );
    result = result && file.write( &m_isSmartStart, sizeof( m_isSmartStart ) );
    result = result && file.write( &m_aggregatedChannelsCount, sizeof( m_aggregatedChannelsCount ) );
    result = result && file.write( &m_isRediscoverRequired, sizeof( m_isRediscoverRequired ) );
    result = result && file.write( &m_needExtendedRediscover, sizeof( m_needExtendedRediscover ) );
    result = result && file.write( &m_capability, sizeof( m_capability ) );

    bool isWakeUpIntervalInitialized = getWakeUpInterval().has_value();
    result = result && file.write( &isWakeUpIntervalInitialized, sizeof( isWakeUpIntervalInitialized ) );
    if( isWakeUpIntervalInitialized ) {
        uint32_t wakeUpInterval = *getWakeUpInterval();
        result = result && file.write( &wakeUpInterval, sizeof( wakeUpInterval ) );
    }

    bool isDeviceSpecificWakeUpIntervalInitialized = m_deviceSpecificWakeUpInterval.has_value();
    result = result && file.write( &isDeviceSpecificWakeUpIntervalInitialized,
                                   sizeof( isDeviceSpecificWakeUpIntervalInitialized ) );
    if( isDeviceSpecificWakeUpIntervalInitialized ) {
        uint32_t wakeUpInterval = *m_deviceSpecificWakeUpInterval;
        result = result && file.write( &wakeUpInterval, sizeof( wakeUpInterval ) );
    }

    const uint8_t channelsCount = getChannelsCount();
    result = result && file.write( &channelsCount, sizeof( channelsCount ) );

    for( uint8_t channelId = ZNET_CHANNEL_ID_ROOT; channelId < channelsCount; ++channelId ) {
        result = result && file.write( &channelId, sizeof( channelId ) );

        const uint8_t supportedClassesCount = getCommandClassCount( channelId );
        const uint8_t unsupportedClassesCount = getUnsupportedCommandClassCount( channelId );
        const uint8_t commandClassesCount = supportedClassesCount + unsupportedClassesCount;
        result = result && file.write( &commandClassesCount, sizeof( commandClassesCount ) );

        for( uint8_t commandClassIndex = 0; commandClassIndex < supportedClassesCount; ++commandClassIndex ) {
            auto commandClass = getCommandClassByIndex( channelId, commandClassIndex );
            if( !commandClass ) {
                continue;
            }

            const uint8_t commandClassId = commandClass->getId();
            result = result && file.write( &commandClassId, sizeof( commandClassId ) );
            const uint8_t commandClassVersion = commandClass->getVersion();
            result = result && file.write( &commandClassVersion, sizeof( commandClassVersion ) );
            const bool isSaveData = true;

            result = result && file.write( &isSaveData, sizeof( isSaveData ) );

            result = result && commandClass->storeData( file );
            if( result == false ) {
                ZNET_LOGE( "Error store cc 0x%x\n", commandClass->getId() );
                break;
            }
        }

        for( uint8_t commandClassIndex = 0; commandClassIndex != unsupportedClassesCount; ++commandClassIndex ) {
            // Note: must be the same format as for supported CCs
            const uint8_t commandClassId = getUnsupportedCommandClassId( channelId, commandClassIndex );
            const uint8_t commandClassVersion = 1;
            const bool hasSavedData = false;

            result = result && file.write( &commandClassId, sizeof( commandClassId ) );
            result = result && file.write( &commandClassVersion, sizeof( commandClassVersion ) );
            result = result && file.write( &hasSavedData, sizeof( hasSavedData ) );
        }
    }

    if( !result )
        ZNET_LOGE( "Failed to store node" );

    return result;
}

bool ZNode::restoreData( IFile& file ) {
    auto result = true;

    auto restoreSecurityMode = [&file]( SecurityMode::Class& mode ) {
        uint8_t value;
        if( file.read( &value, sizeof( value ) ) ) {
            mode = SecurityMode::fromByte( value );
            return true;
        }

        return false;
    };

    int lastFormatVersion = 0;
    result = result && file.read( &lastFormatVersion, sizeof( lastFormatVersion ) );

    result = result && file.read( &m_id, sizeof( m_id ) );
    result = result && file.read( &m_manufacturerId, sizeof( m_manufacturerId ) );
    result = result && file.read( &m_productId, sizeof( m_productId ) );
    result = result && file.read( &m_productType, sizeof( m_productType ) );
    result = result && file.read( &m_genericType, sizeof( m_genericType ) );
    result = result && file.read( &m_basicType, sizeof( m_basicType ) );
    result = result && file.read( &m_specificType, sizeof( m_specificType ) );
    result = result && restoreSecurityMode( m_security );
    result = result && restoreSecurityMode( m_securityClassStarted );
    result = result && file.read( &m_zwavePlusIconType, sizeof( m_zwavePlusIconType ) );

    // Smart Start was introduced in v8
    if( lastFormatVersion < majorFormatVersion_v8 ) {
        m_isSmartStart = false;
    }
    else {
        result = result && file.read( &m_isSmartStart, sizeof( m_isSmartStart ) );
    }

    if( lastFormatVersion >= majorFormatVersion_v18 ) {
        result = result && file.read( &m_aggregatedChannelsCount, sizeof( m_aggregatedChannelsCount ) );
    }

    if( lastFormatVersion >= majorFormatVersion_v21 ) {
        result = result && file.read( &m_isRediscoverRequired, sizeof( m_isRediscoverRequired ) );
    }
    else {
        m_isRediscoverRequired = false;
    }

    if( lastFormatVersion >= majorFormatVersion_v22 ) {
        result = result && file.read( &m_needExtendedRediscover, sizeof( m_needExtendedRediscover ) );
    }

    if( lastFormatVersion >= majorFormatVersion_v25 ) {
        result = result && file.read( &m_capability, sizeof( m_capability ) );
    }

    if( lastFormatVersion >= majorFormatVersion_v14 ) {
        bool isWakeUpIntervalInitialized;
        result = result && file.read( &isWakeUpIntervalInitialized, sizeof( isWakeUpIntervalInitialized ) );
        if( isWakeUpIntervalInitialized ) {
            uint32_t wakeUpInterval;
            result = result && file.read( &wakeUpInterval, sizeof( wakeUpInterval ) );
            setWakeUpInterval( wakeUpInterval );
        }
    }

    if( lastFormatVersion >= majorFormatVersion_v16 ) {
        bool isSeviceSpecificWakeUpIntervalInitialized;
        result = result && file.read( &isSeviceSpecificWakeUpIntervalInitialized,
                                      sizeof( isSeviceSpecificWakeUpIntervalInitialized ) );
        if( isSeviceSpecificWakeUpIntervalInitialized ) {
            uint32_t wakeUpInterval;
            result = result && file.read( &wakeUpInterval, sizeof( wakeUpInterval ) );
            m_deviceSpecificWakeUpInterval = wakeUpInterval;
        }
    }

    uint8_t channelsCount = 0;
    result = result && file.read( &channelsCount, sizeof( channelsCount ) );
    if( channelsCount > 1 ) {
        createAdditionalChannels( channelsCount - 1 );  // ZNET_CHANNEL_ID_ROOT already exists in node
    }

    for( uint8_t channelId = ZNET_CHANNEL_ID_ROOT; channelId < channelsCount; ++channelId ) {
        result = result && file.read( &channelId, sizeof( channelId ) );

        uint8_t commandClassesCount = 0;
        result = result && file.read( &commandClassesCount, sizeof( commandClassesCount ) );

        for( uint8_t i = 0; i < commandClassesCount; ++i ) {
            uint8_t commandClassId = 0;
            result = result && file.read( &commandClassId, sizeof( commandClassId ) );

            uint8_t commandClassVersion = 0;
            result = result && file.read( &commandClassVersion, sizeof( commandClassVersion ) );

            bool hasSavedData = false;
            result = result && file.read( &hasSavedData, sizeof( hasSavedData ) );

            const auto isSecurityCc =
                commandClassId == ZNET_COMMAND_CLASS_SECURITY || commandClassId == ZNET_COMMAND_CLASS_SECURITY_2;

            if( lastFormatVersion <= majorFormatVersion_v19 && commandClassVersion == 1 ) {
                const auto isCcSupportAddedAfterV19 = commandClassId == ZNET_COMMAND_CLASS_ANTITHEFT ||
                                                      commandClassId == ZNET_COMMAND_CLASS_ANTITHEFT_UNLOCK ||
                                                      isSecurityCc;
                // Unsupported command classes had been saved before switching to 20 version with wrong field
                // hasSavedData=true. So we have to fix this field for newly added classes

                if( isCcSupportAddedAfterV19 ) {
                    hasSavedData = false;
                }
            }

            if( hasSavedData ) {
                ZNode::ClassPtr commandClass = addCommandClass( channelId, commandClassId, commandClassVersion );
                if( !commandClass ) {
                    continue;
                }

                result = result && commandClass->restoreData( file, lastFormatVersion );
                if( result == false ) {
                    ZNET_LOGE( "Error restore cc 0x%x, result %d\n", commandClass->getId(), result );
                    break;
                }

                if( !m_isRediscoverRequired &&
                    commandClass->isRediscoverRequired() /*Not completely restored, need interview again*/ ) {
                    m_isRediscoverRequired = true;
                }
            }
            else {
                addCommandClass( channelId, commandClassId, commandClassVersion, false /*implicitly not supported*/ );

                if( !m_isRediscoverRequired &&
                    !isSecurityCc /*security cc has been added onl for certification casesy*/ &&
                    canCreateCommandClass( commandClassId ) /*is newly added cc*/ ) {
                    m_isRediscoverRequired = true;
                }
            }
        }
    }

    m_jobs.setID( getId() );

    if( !result )
        ZNET_LOGE( "Failed to restore node" );

    return result;
}

void ZNode::setTimeOffset( TimeDifference offset ) {
    m_timeOffset = offset;
}

ZNode::TimeDifference ZNode::getTimeOffset() const {
    return m_timeOffset;
}

void ZNode::setRequestDelay( RequestDelay requestDelay ) {
    m_requestDelay = requestDelay;
}

ZNode::RequestDelay ZNode::getRequestDelay() const {
    return m_requestDelay;
}

RequestsManager& ZNode::getRequestsManager() const {
    if( !m_requestsManager ) {
        ZNET_LOGE( "requests manager is not set for node!!!\n" );
        assert( false );
    }

    return *m_requestsManager;
}

IAssociationsConfigurator& ZNode::getAssociationsConfigurator() {
    if( !m_associationsConfigurator ) {
        ZNET_LOGE( "association configurator is not set for node!!!\n" );
        assert( false );
    }

    return *m_associationsConfigurator;
}

JobsHolder& ZNode::getJobs() {
    return m_jobs;
}

ZChannel::ZChannel( const uint8_t channelId, ZNode& parentNode )
: m_id( channelId )
, m_parentNode( parentNode ) {}

ZChannel::ZChannel( ZChannel&& other ) noexcept
: m_id( other.m_id )
, m_parentNode( other.m_parentNode )
, m_channelClasses( std::move( other.m_channelClasses ) ) {
    for( auto& commandClass : m_channelClasses ) {
        assert( commandClass );
        commandClass->setParentChannel( *this );
    }
}

znet_node_channel_id_t ZChannel::getId() const {
    return m_id;
}

ZChannel::ClassPtr ZChannel::addCommandClass( ClassPtr commandClass ) {
    if( commandClass ) {
        m_channelClasses.push_back( std::move( commandClass ) );
        return m_channelClasses.back();
    }

    return {};
}

ZChannel::ClassPtr ZChannel::getCommandClass( const znet_command_class_t ccid ) const {
    for( auto& commandClass : m_channelClasses ) {
        assert( commandClass );
        if( ccid == commandClass->getId() ) {
            return commandClass;
        }
    }
    return {};
}

ZChannel::ClassPtr ZChannel::getCommandClassByIndex( const uint8_t index ) const {
    if( index < m_channelClasses.size() ) {
        return m_channelClasses[index];
    }

    return {};
}

uint8_t ZChannel::getCommandClassCount() const {
    return static_cast<uint8_t>( m_channelClasses.size() );
}

void ZChannel::addUnsupportedCommandClass( uint8_t id ) {
    if( std::find( m_unsupportedClasses.begin(), m_unsupportedClasses.end(), id ) == m_unsupportedClasses.end() )
        m_unsupportedClasses.push_back( id );
}

uint8_t ZChannel::getUnsupportedCommandClassCount() const {
    return m_unsupportedClasses.size();
}

uint8_t ZChannel::getUnsupportedCommandClassId( uint8_t index ) const {
    if( index < m_unsupportedClasses.size() ) {
        return m_unsupportedClasses[index];
    }
    else {
        ZNET_LOGE(
            "try get Unsupported Command Class Id by index = %d, size = %d\n", index, m_unsupportedClasses.size() );
        return 0;
    }
}

ZNode& ZChannel::getParentNode() {
    return m_parentNode;
}

}  // namespace znet
