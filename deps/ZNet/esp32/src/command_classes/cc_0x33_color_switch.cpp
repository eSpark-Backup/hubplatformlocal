#include "znet/cc_0x33_color_switch.hpp"
#include "znet/znet_node.hpp"
#include "znet/znet_requests_manager.hpp"
#include "znet/znet_utils.hpp"

extern "C" {
#include "znet_log.h"
}

#include <algorithm>
#include <numeric>
#include <cassert>

namespace znet {

namespace {

struct ComponentIdComparator {
    bool operator()( const ColorComponentInfo& component ) const {
        return component.id == componentId;
    }

    ColorComponentID componentId;
};

uint16_t getComponentsMask( const ColorComponents& components ) {
    return std::accumulate(
        components.begin(), components.end(), uint16_t( 0 ), []( uint16_t mask, const znet_cl_component_t& component ) {
            return mask | ( 1u << component.component_id );
        } );
}

uint16_t getComponentsMask( const ColorComponentInfos& components ) {
    return std::accumulate(
        components.begin(), components.end(), uint16_t( 0 ), []( uint16_t mask, const ColorComponentInfo& component ) {
            return mask | ( 1u << static_cast<uint8_t>( component.id ) );
        } );
}

void fixComponents( uint16_t supportedComponentsMask, ColorComponents& components ) {
    const auto componentsMask = getComponentsMask( components );

    utils::forEachBit(
        reinterpret_cast<const uint8_t*>( &componentsMask ), sizeof( componentsMask ), [&]( uint8_t componentId ) {
            if( !( supportedComponentsMask & ( 1u << componentId ) ) ) {
                auto res =
                    std::find_if( components.begin(), components.end(), [&componentId]( znet_cl_component_t& c ) {
                        return c.component_id == componentId;
                    } );
                if( res != components.end() ) {
                    components.erase( res );
                }
            }
        } );

    utils::forEachBit( reinterpret_cast<const uint8_t*>( &supportedComponentsMask ),
                       sizeof( supportedComponentsMask ),
                       [&]( uint8_t componentId ) {
                           if( !( componentsMask & ( 1u << componentId ) ) ) {
                               znet_cl_component_t color;
                               color.component_id = componentId;
                               color.value = 0;
                               components.push_back( color );
                           }
                       } );
}

}  // namespace

ZCc0x33ColorSwitch::ComponentsCache::ComponentsCache( ZCc0x33ColorSwitch& relatedCC )
: m_relatedCC( relatedCC ) {}

void ZCc0x33ColorSwitch::ComponentsCache::addComponent( const ColorComponentInfo& component ) {
    const auto it = std::find_if( m_components.begin(), m_components.end(), ComponentIdComparator{ component.id } );
    if( it != m_components.end() ) {
        *it = component;
    }
    else {
        m_components.push_back( component );
    }
}

bool ZCc0x33ColorSwitch::ComponentsCache::isCompleted() const {
    return ( m_relatedCC.m_supportedComponentsMask & ~getComponentsMask( m_components ) ) == 0;
}

bool ZCc0x33ColorSwitch::ComponentsCache::hasComponent( ColorComponentID componentId ) const {
    assert( m_relatedCC.isComponentSupported( componentId ) );

    return std::any_of( m_components.begin(), m_components.end(), ComponentIdComparator{ componentId } );
}

ColorComponentInfos ZCc0x33ColorSwitch::ComponentsCache::takeComponents() {
    return std::move( m_components );
}

ZCc0x33ColorSwitch::ZCc0x33ColorSwitch( ZChannel& parent, znet_command_class_version_t version )
: ZClass( parent, version )
, m_componentsCache( *this ) {}

bool ZCc0x33ColorSwitch::setRgb( ColorComponents& components ) {
    fixComponents( m_supportedComponentsMask, components );

    const auto res = znet_node_cmd_color_switch_set(
        getNodeId(), getChannelId(), static_cast<uint8_t>( components.size() ), components.data(), getVersion() );
    if( ZNET_ERR_OK != res ) {
        return false;
    }

    return true;
}

bool ZCc0x33ColorSwitch::setRgbMulticast( const uint8_t nodeQuantity,
                                          const znet_node_id_t* listNodeId,
                                          ColorComponents& components,
                                          const bool sendSingleCasts ) {
    fixComponents( m_supportedComponentsMask, components );

    const auto res = znet_node_cmd_color_switch_set_multi( nodeQuantity,
                                                           listNodeId,
                                                           getChannelId(),
                                                           static_cast<uint8_t>( components.size() ),
                                                           components.data(),
                                                           sendSingleCasts );
    return ZNET_ERR_OK == res;
}

bool ZCc0x33ColorSwitch::requestValue( const RequestValue& request ) {
    if( m_status == Status::Polling ) {
        ZNET_LOGW( "cc_0x33: Cannot start requestValue, actual status is polling\n" );
        return false;
    }

    m_componentsCache.takeComponents();
    m_status = Status::Polling;

    const auto result =
        m_supportedComponentsMask &&
        requestNextComponent(
            request.resultCallback ? std::make_shared<RequestValue::Callback>( request.resultCallback ) : nullptr,
            request.jobContext );

    if( !result ) {
        m_status = Status::Idle;
    }

    return result;
}

bool ZCc0x33ColorSwitch::requestNextComponent( RequestCallbackPtr callback,
                                               WeakJobContext jobContext,
                                               uint8_t componentId ) {
    const auto firstBit = utils::findFirstBit( reinterpret_cast<const uint8_t*>( &m_supportedComponentsMask ),
                                               sizeof( m_supportedComponentsMask ),
                                               componentId );

    auto onFinished = [=]( RequestFailTypeOpt failtTypeOpt ) {
        m_status = Status::Idle;
        onRequestsFinished( callback, failtTypeOpt );
    };

    if( !firstBit ) {
        onFinished( {} );
        return true;
    }

    componentId = *firstBit;

    return getParentNode().getRequestsManager().doRequest(
        ColorSwitchValueRequest( getNodeId(), getChannelId(), componentId ),
        jobContext,
        [=]( const znet_color_switch_report_t& ) {
            if( !requestNextComponent( callback, jobContext, componentId + 1 ) ) {
                onFinished( RequestFailType::SendingError );
            }
        },
        onFinished );
}

bool ZCc0x33ColorSwitch::interview( InterviewCallback callback, WeakJobContext context ) {
    return singleInterviewRequestImpl(
        *this,
        ColorSwitchSupportedTypesRequest( getNodeId(), getChannelId() ),
        context,
        [this]( const znet_color_switch_supported_report_t& report ) {
            memcpy( &m_supportedComponentsMask,
                    report.component_mask,
                    std::min( report.component_mask_count, uint8_t( sizeof( m_supportedComponentsMask ) ) ) );
        },
        std::move( callback ) );
}

bool ZCc0x33ColorSwitch::isComponentSupported( ColorComponentID componentId ) const {
    assert( componentId < ColorComponentID::ColorComponentsCount );
    return ( m_supportedComponentsMask & ( 1u << static_cast<uint8_t>( componentId ) ) ) != 0;
}

bool ZCc0x33ColorSwitch::setLevelChange( ColorComponentID componentId,
                                         ValueChangeCommand command,
                                         const SecondsOpt& duration ) {
    if( command == ValueChangeCommand::Stop ) {
        return stopLevelChange( componentId );
    }
    else {
        return startLevelChange( componentId, command == ValueChangeCommand::Down, {}, duration );
    }
}

bool ZCc0x33ColorSwitch::startLevelChange( ColorComponentID componentId,
                                           bool downDirection,
                                           utils::optional<uint8_t> startLevel,
                                           const SecondsOpt& duration ) {
    return isComponentSupported( componentId ) &&
           znet_node_cmd_color_switch_start_level_change( getNodeId(),
                                                          getChannelId(),
                                                          downDirection,
                                                          !startLevel,
                                                          static_cast<uint8_t>( componentId ),
                                                          startLevel ? *startLevel : 0,
                                                          utils::actuatorSetTimeFromSeconds( duration ),
                                                          getVersion() ) == ZNET_ERR_OK;
}

bool ZCc0x33ColorSwitch::stopLevelChange( ColorComponentID componentId ) {
    return isComponentSupported( componentId ) &&
           znet_node_cmd_color_switch_stop_level_change(
               getNodeId(), getChannelId(), static_cast<uint8_t>( componentId ) ) == ZNET_ERR_OK;
}

std::vector<ColorComponentID> ZCc0x33ColorSwitch::getSupportedColors() const {
    std::vector<ColorComponentID> components;
    components.reserve( utils::getBitsCount( reinterpret_cast<const uint8_t*>( &m_supportedComponentsMask ),
                                             sizeof( m_supportedComponentsMask ) ) );

    utils::forEachBit( reinterpret_cast<const uint8_t*>( &m_supportedComponentsMask ),
                       sizeof( m_supportedComponentsMask ),
                       [&]( uint8_t index ) { components.push_back( static_cast<ColorComponentID>( index ) ); } );

    return components;
}

ZCc0x33ColorSwitch::ComponentsCache& ZCc0x33ColorSwitch::getUpdatedComponentsCache() {
    return m_componentsCache;
}

bool ZCc0x33ColorSwitch::storeData( IFile& file ) const {
    return file.write( &m_supportedComponentsMask, sizeof( m_supportedComponentsMask ) );
}

bool ZCc0x33ColorSwitch::restoreData( IFile& file, int version ) {
    return file.read( &m_supportedComponentsMask, sizeof( m_supportedComponentsMask ) );
}

}  // namespace znet
