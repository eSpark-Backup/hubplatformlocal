
#pragma once

#include <znet/znet.h>
#include <znet/znet_command_class.hpp>
#include <znet/znet_jobs_holder.hpp>
#include <znet/enums/security_mode.hpp>

#include <chrono>

namespace znet {

namespace ut {
class ZNodeAccessor;
}

class Request;
class ZChannel;
class ZNode;
class IAssociationsConfigurator;
class RequestsManager;
class ReachableCheckTimeoutSettings;

enum class ReachableState { Unknown = 0x00, Reachable = 0x01, Unreachable = 0x02 };
constexpr int majorFormatVersion_v3 = 3;
constexpr int majorFormatVersion_v4 = 4;
constexpr int majorFormatVersion_v5 = 5;
constexpr int majorFormatVersion_v6 = 6;
constexpr int majorFormatVersion_v7 = 7;
constexpr int majorFormatVersion_v8 = 8;
constexpr int majorFormatVersion_v9 = 9;
constexpr int majorFormatVersion_v10 = 10;
constexpr int majorFormatVersion_v11 = 11;
constexpr int majorFormatVersion_v12 = 12;
constexpr int majorFormatVersion_v13 = 13;
constexpr int majorFormatVersion_v14 = 14;
constexpr int majorFormatVersion_v15 = 15;
constexpr int majorFormatVersion_v16 = 16;
constexpr int majorFormatVersion_v17 = 17;
constexpr int majorFormatVersion_v18 = 18;
constexpr int majorFormatVersion_v19 = 19;
constexpr int majorFormatVersion_v20 = 20;
constexpr int majorFormatVersion_v21 = 21;
constexpr int majorFormatVersion_v22 = 22;
constexpr int majorFormatVersion_v23 = 23;
constexpr int majorFormatVersion_v24 = 24;
constexpr int majorFormatVersion_v25 = 25;
constexpr int majorFormatVersion_v26 = 26;

class ZNode {
    friend class ut::ZNodeAccessor;

public:
    static const int WaitingDelayMs;
    static const int WaitingDelayS0Ms;

    using ClassPtr = std::shared_ptr<ZClass>;
    using TimeDifference = std::chrono::minutes;
    using RequestDelay = std::chrono::milliseconds;

    struct FirmwareInfo {
        std::string id;
        std::string version;
    };

public:
    ZNode( znet::RequestsManager* requestManager,
           IAssociationsConfigurator* associationsConfigurator,
           ReachableCheckTimeoutSettings* reachableCheckTimeoutSettings );

    virtual ~ZNode() = default;

    void init( znet_node_id_t nodeId, SecurityMode::Class security = SecurityMode::Class::None );

    void reset();

    znet_node_id_t getId() const;

    ClassPtr addCommandClass( const znet_node_channel_id_t channelId,
                              const znet_command_class_t ccid,
                              const znet_command_class_version_t = 1,
                              const bool supported = true );

    template <typename ClassType>
    std::shared_ptr<ClassType> getNextCommandClass( std::shared_ptr<ClassType> classInPreviousChannel ) const;

    template <typename ClassType>
    std::shared_ptr<ClassType> getNextCommandClass(
        znet_node_channel_id_t prevChannel = ZNET_CHANNEL_ID_INVALID ) const;

    template <typename ClassType>
    std::shared_ptr<ClassType> getCommandClass( const znet_node_channel_id_t channelId ) const;

    ClassPtr getCommandClass( const znet_node_channel_id_t channelId, const znet_command_class_t ccid ) const;

    ClassPtr getCommandClassByIndex( const znet_node_channel_id_t channelId, const uint8_t index ) const;
    uint8_t getCommandClassCount( const znet_node_channel_id_t channelId ) const;

    uint8_t getUnsupportedCommandClassId( znet_node_channel_id_t channelId, uint8_t idx ) const;
    uint8_t getUnsupportedCommandClassCount( znet_node_channel_id_t channelId ) const;

    void createAdditionalChannels( uint8_t channelsCount );
    void setAggregatedChannelsCount( uint8_t channelsCount );

    uint8_t getChannelsCount() const;
    uint8_t getAggregatedChannelsCount() const;
    uint8_t getAllChannelsCount() const;

    void setManufacturerSpecific( uint16_t manufacturerId, uint16_t productId, uint16_t productType );
    void setNodeInfoExtTypes( uint8_t genericType, uint8_t basicType, uint8_t specificType );

    void setSecurity( SecurityMode::Class security );
    SecurityMode::Class getSecurity() const;
    void setSecurityClassStarted( SecurityMode::Class security );
    SecurityMode::Class getSecurityClassStarted() const;

    void setZwavePlusIconType( uint16_t type );
    uint16_t getZwavePlusIconType() const;

    uint16_t getManufacturerId() const;
    uint16_t getProductType() const;
    uint16_t getProductId() const;
    uint8_t getGenericType() const;
    uint8_t getBasicType() const;
    uint8_t getSpecificType() const;

    bool isSmartStart() const;
    void setIsSmartStart( bool isSmartStart );

    virtual void setReachable( ReachableState isReachable );

    ReachableState isReachable() const;

    bool isBatteryPowered() const;

    bool isWakeup( bool forced = false ) const;

    void setIsWokenUp( bool isWokenUp = false );
    bool isWokenUp() const;

    void setRoleType( uint8_t roleType );
    uint8_t getRoleType() const;

    void setForcedRequests( bool hasForcedRequests = false );
    bool hasForcedRequests() const;

    void setWakeUpInterval( uint32_t interval );
    void setDeviceSpecificWakeUpInterval( uint32_t interval );
    void updateWakeUpInterval();
    utils::optional<uint32_t> getWakeUpInterval() const;

    void loop();

    std::chrono::steady_clock::time_point getLastUpdateTime() const;

    bool storeData( IFile& file ) const;
    bool restoreData( IFile& file );

    void setTimeOffset( TimeDifference offset );

    TimeDifference getTimeOffset() const;

    void setRequestDelay( RequestDelay requestDelay );

    RequestDelay getRequestDelay() const;

    RequestsManager& getRequestsManager() const;

    IAssociationsConfigurator& getAssociationsConfigurator();

    JobsHolder& getJobs();

    void onRediscoverStarted();

    void onRediscoverFinished();

    bool isRediscoverStarted() const;

    bool isRediscoverRequired() const;
    void setRediscoverRequired( bool value );

    bool needExtendedRediscover() const;
    void setNeedExtendedRediscover( bool value );

    void setCapability( uint8_t capability );
    uint16_t getCommandSpan() const;

protected:
    ZNode( const ZNode& node ) = delete;
    ZNode& operator=( const ZNode& node ) = delete;

    ZNode( ZNode&& node ) = delete;
    ZNode& operator=( ZNode&& node ) = delete;

private:
    IAssociationsConfigurator* m_associationsConfigurator;

    znet_node_id_t m_id;
    uint16_t m_manufacturerId;
    uint16_t m_productType;
    uint16_t m_productId;
    uint8_t m_genericType;
    uint8_t m_basicType;
    uint8_t m_specificType;
    SecurityMode::Class m_security;
    SecurityMode::Class m_securityClassStarted;
    uint16_t m_zwavePlusIconType;
    uint8_t m_flags;
    uint8_t m_capability;
    bool m_isSmartStart;

    utils::optional<uint32_t> m_activeWakeUpInterval;
    utils::optional<uint32_t> m_deviceSpecificWakeUpInterval;

    std::vector<ZChannel> m_nodeChannels;
    uint8_t m_aggregatedChannelsCount;

    std::chrono::steady_clock::time_point m_lastDataUpdateTime;

    TimeDifference m_timeOffset;
    RequestDelay m_requestDelay;

    RequestsManager* m_requestsManager;
    ReachableCheckTimeoutSettings* m_reachableCheckTimeoutSettings;

    ReachableState m_isReachable = ReachableState::Unknown;
    bool m_isWokenUp;
    uint8_t m_roleType;
    bool m_needForcedRequests;
    JobsHolder m_jobs;
    bool m_isRediscover = false;
    bool m_isRediscoverRequired = false;
    bool m_needExtendedRediscover = false;
};

class ZChannel final {
    friend class ut::ZNodeAccessor;

public:
    ZChannel( const uint8_t channel_id, ZNode& parentNode );

    ZChannel( ZChannel&& ch ) noexcept;

    znet_node_channel_id_t getId() const;

    using ClassPtr = std::shared_ptr<ZClass>;

    ClassPtr addCommandClass( ClassPtr commandClass );
    ClassPtr getCommandClass( const znet_command_class_t ccid ) const;
    ClassPtr getCommandClassByIndex( const uint8_t index ) const;
    uint8_t getCommandClassCount() const;

    void addUnsupportedCommandClass( uint8_t id );
    uint8_t getUnsupportedCommandClassCount() const;
    uint8_t getUnsupportedCommandClassId( uint8_t index ) const;

    ZNode& getParentNode();

    IAssociationsConfigurator& getAssociationsConfigurator();

private:
    ZChannel() = delete;

    ZChannel( const ZChannel& ch ) = delete;

    ZChannel& operator=( const ZChannel& ch ) = delete;

    ZChannel& operator=( ZChannel&& ch ) = delete;

private:
    znet_node_channel_id_t m_id;
    ZNode& m_parentNode;
    std::vector<ClassPtr> m_channelClasses;
    std::vector<uint8_t> m_unsupportedClasses;
};

template <typename Class>
std::shared_ptr<Class> ZNode::getCommandClass( const znet_node_channel_id_t channelId ) const {
    using ID = typename Class::ID;
    return std::static_pointer_cast<Class>( getCommandClass( channelId, ID::value ) );
}

template <typename Class>
std::shared_ptr<Class> ZNode::getNextCommandClass( std::shared_ptr<Class> classInPreviousChannel ) const {
    assert( !classInPreviousChannel || &classInPreviousChannel->getParentNode() == this );
    return getNextCommandClass<Class>( classInPreviousChannel ? classInPreviousChannel->getParentChannel().getId() :
                                                                ZNET_CHANNEL_ID_INVALID );
}

template <typename Class>
std::shared_ptr<Class> ZNode::getNextCommandClass( znet_node_channel_id_t prevChannel ) const {
    for( auto channelId = prevChannel == ZNET_CHANNEL_ID_INVALID ? ZNET_CHANNEL_ID_ROOT : prevChannel + 1;
         channelId < m_nodeChannels.size();
         ++channelId ) {
        if( auto cc = getCommandClass<Class>( channelId ) )
            return cc;
    }
    return nullptr;
}

}  // namespace znet
