#include "znet_handler_values.hpp"
#include "znet_impl.hpp"
#include "znet/znet.h"
#include "znet/znet_node.hpp"
#include "znet/cc_0x80_battery.hpp"
#include "znet/cc_0x84_wakeup.hpp"
extern "C" {
#include "znet_log.h"
}

#include "znet/znet_utils.hpp"

namespace znet {

namespace {

/* @brief
 * ( 3s (per each retry) * 5 (number of retries) ) + 10s ( some additional interval )
 * In perfect, interval is not needed becasuse CurrentValuesHandler has been refactored via requests manager.
 * Interval is still used because of potential bugs.
 */
const std::chrono::seconds CurrentValuesHandlerTimeout(
    35 );  //  ( 3s (per each retry) * 5 (number of retries) ) + 5 ( some additional interval )
}  // namespace

CurrentValuesHandler::CurrentValuesHandler( ZNetImpl& parent )
: MonitoringBaseHandler( parent )
, NodesProcessor( parent.getOwner(), parent.getOwner(), CurrentValuesHandlerTimeout ) {
    // Initialize Request Priority Queue
    m_requestsCcOrder.clear();
    pushToRequestOrder( ZNET_COMMAND_CLASS_ASSOCIATION );
    pushToRequestOrder( ZNET_COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION );
    pushToRequestOrder( ZNET_COMMAND_CLASS_CONFIGURATION );
    pushToRequestOrder( ZNET_COMMAND_CLASS_SENSOR_BINARY );
    pushToRequestOrder( ZNET_COMMAND_CLASS_SENSOR_ALARM );
    pushToRequestOrder( ZNET_COMMAND_CLASS_NOTIFICATION );
    pushToRequestOrder( ZNET_COMMAND_CLASS_SWITCH_BINARY );
    pushToRequestOrder( ZNET_COMMAND_CLASS_SENSOR_MULTILEVEL );
    pushToRequestOrder( ZNET_COMMAND_CLASS_SWITCH_MULTILEVEL );
    pushToRequestOrder( ZNET_COMMAND_CLASS_DOOR_LOCK );
    pushToRequestOrder( ZNET_COMMAND_CLASS_THERMOSTAT_MODE );
    pushToRequestOrder( ZNET_COMMAND_CLASS_THERMOSTAT_OPERATING_STATE );
    pushToRequestOrder( ZNET_COMMAND_CLASS_THERMOSTAT_FAN_MODE );
    pushToRequestOrder( ZNET_COMMAND_CLASS_THERMOSTAT_FAN_STATE );
    pushToRequestOrder( ZNET_COMMAND_CLASS_THERMOSTAT_SETBACK );
    pushToRequestOrder( ZNET_COMMAND_CLASS_THERMOSTAT_SETPOINT );
    pushToRequestOrder( ZNET_COMMAND_CLASS_BARRIER_OPERATOR );
    pushToRequestOrder( ZNET_COMMAND_CLASS_SOUND_SWITCH );
    pushToRequestOrder( ZNET_COMMAND_CLASS_SWITCH_COLOR );
    pushToRequestOrder( ZNET_COMMAND_CLASS_METER );
    pushToRequestOrder( ZNET_COMMAND_CLASS_BATTERY );
    pushToRequestOrder( ZNET_COMMAND_CLASS_WAKE_UP );
    pushToRequestOrder( ZNET_COMMAND_CLASS_BASIC );
}

void CurrentValuesHandler::initialize() {
    ZNetHandler::initialize();

    m_parentMesh.setState( IZNet::Mode::ValuesGetting, ModeStatus::Started );

    // init FRT here on RESET! TODO: refactor it!!!!
    utils::forEachBit( getProcessingNodeIds().mask, NodeIdsSet::Size, [this]( uint8_t nodeId ) {
        ZNode* node = m_parentMesh.onFindNode( nodeId );
        if( node != nullptr ) {
            if( node->hasForcedRequests() ) {
                znet_forced_request_timer_create( 0 );
            }
        }
    } );

    startNodesProcessing();
}

bool CurrentValuesHandler::onLoop() {
    if( !isInProgress() ) {
        m_parentMesh.activateMode( IZNet::Mode::Monitoring );
        m_parentMesh.setState( IZNet::Mode::ValuesGetting, ModeStatus::Finished );
    }
    return true;
}

void CurrentValuesHandler::onDeactivated() {
    stopNodesProcessing();
}

void CurrentValuesHandler::finalize( ErrorOpt ) {}

void CurrentValuesHandler::pushToRequestOrder( znet_command_class_t commandClassId ) {
    m_requestsCcOrder.push_back( commandClassId );
}

bool CurrentValuesHandler::processNode( ZNode& currentNode ) {
    return processCommandClasses();
}

bool CurrentValuesHandler::processCommandClasses( znet_node_channel_id_t channelId, uint8_t ccIndex ) {
    auto node = getCurrentNode();
    if( node == nullptr ) {
        ZNET_LOGE( "%s could not find current node info  index = %d\n", __func__, ccIndex );
        return false;
    }

    const auto nodeId = node->getId();

    for( ; channelId < node->getChannelsCount(); ++channelId ) {  // iteration over channel
        for( ; ccIndex < m_requestsCcOrder.size(); ++ccIndex ) {  // iteration over classes
            auto cc = node->getCommandClass( channelId, m_requestsCcOrder[ccIndex] );

            if( !cc ) {
                continue;
            }

            ZNET_LOGD( "Requesting values: cc 0x%x, node %u, ch %u\n", m_requestsCcOrder[ccIndex], nodeId, channelId );

            RequestValue request;
            request.jobContext = getJobContext();
            request.resultCallback = [=]( RequestFailTypeOpt failOpt ) {
                if( nodeId != getCurrentNodeId() ) {
                    ZNET_LOGE(
                        "Something went wrong, report received for unexpected node. Actual node id: %u, expected %u\n",
                        getCurrentNodeId(),
                        nodeId );
                    return;
                }

                if( failOpt ) {
                    ZNET_LOGE( "CurrentValuesHandler, Failed cc processing. CC = %d, ch = %d, fail = %d\n",
                               cc->getId(),
                               channelId,
                               *failOpt );
                }

                if( failOpt && *failOpt == RequestFailType::ReportedFail ) {
                    ZNET_LOGE( "Finish values geting due to reported fail\n" );
                    onNodeProcessed( err::values::values_getting_failed( channelId, cc->getId(), *failOpt ) );
                }
                else {
                    if( !processCommandClasses( channelId, ccIndex + 1 ) ) {
                        onNodeProcessed( {} );
                    }
                }
            };

            if( cc->canRequest() && cc->requestValue( request ) ) {
                updateTimePoint();  // TODO: do we need it? Remove if possible. See CurrentValuesHandlerTimeout
                return true;
            }
        }

        ccIndex = 0;
    }

    return false;
}

void CurrentValuesHandler::onNodeProcessed( ErrorOpt errorMessage ) {
    if( auto node = getCurrentNode() ) {
        if( node->isWakeup() && node->isWokenUp() ) {
            ZNET_LOGD( "finalize CurrentValuesHandler: send wakeup_sleep() to node(%d)\n", node->getId() );
            node->setIsWokenUp( false );
            znet_node_cmd_wakeup_sleep( node->getId(), ZNET_CHANNEL_ID_ROOT );
        }
    }

    NodesProcessor::onNodeProcessed( std::move( errorMessage ) );
}

bool CurrentValuesHandler::canHandleNode( znet_node_id_t nodeId ) const {
    return getCurrentNodeId() == nodeId || nodeId == 0 || nodeId == 0xFF;
}

void CurrentValuesHandler::updateReachableState( ZNode& node, bool forceNotify ) {
    MonitoringBaseHandler::updateReachableState( node );
    updateTimePoint();  // TODO: do we need it? Remove if possible. See CurrentValuesHandlerTimeout
}

const std::vector<znet_command_class_t>& CurrentValuesHandler::getRequestsCcOrder() const {
    return m_requestsCcOrder;
}

}  // namespace znet
