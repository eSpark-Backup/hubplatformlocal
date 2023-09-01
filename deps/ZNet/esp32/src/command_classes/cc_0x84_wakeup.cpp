#include "znet/cc_0x84_wakeup.hpp"

#include "znet/znet_node.hpp"
#include "znet/znet_requests_manager.hpp"

extern "C" {
#include "znet_log.h"
}

namespace znet {

const uint32_t ZCc0x84WakeUp::DefaultWakeUpInterval = 5400;

bool ZCc0x84WakeUp::interview( InterviewCallback callback, WeakJobContext context ) {
    if( getVersion() <= 1 ) {
        return false;
    }

    return singleInterviewRequestImpl(
        *this,
        WakeupCapabilitiesRequest( getNodeId(), getChannelId() ),
        context,
        [=]( const znet_wakeup_capabilities_report_t& value ) {
            setWakeUpCapabilities( value.min_wakeup_interval, value.max_wakeup_interval, value.step_wakeup_interval );
        },
        std::move( callback ) );
}

bool ZCc0x84WakeUp::requestValue( const RequestValue& request ) {
    const bool result = requestValueImpl( *this, WakeupIntervalRequest( getNodeId(), getChannelId() ), request );

    if( !result ) {
        ZNET_LOGE( "wakeup interval request fail, node %u\n", getNodeId() );
    }
    return result;
}

bool ZCc0x84WakeUp::requestWakeupInterval( std::function<void( uint32_t, znet_node_id_t )> onSuccess,
                                           OnFailCallback onFail,
                                           WeakJobContext context ) {
    return getParentNode().getRequestsManager().doRequest(
        WakeupIntervalRequest( getNodeId(), getChannelId() ), context, std::move( onSuccess ), std::move( onFail ) );
}

uint32_t ZCc0x84WakeUp::normalizeWakeUpInterval( uint32_t wakeup_interval ) const {
    if( wakeup_interval && m_hasCapabilities ) {
        if( m_step ) {
            const uint32_t tempval = wakeup_interval / m_step;  // to set value in miltiplies of step
            wakeup_interval = tempval * m_step;
        }

        if( wakeup_interval > m_maxWakeUpInterval ) {
            wakeup_interval = m_maxWakeUpInterval;
        }
        else if( wakeup_interval < m_minWakeUpInterval ) {
            wakeup_interval = m_minWakeUpInterval;
        }
    }

    return wakeup_interval;
}

bool ZCc0x84WakeUp::setWakeUpInterval( uint32_t wakeupInterval, bool changeCacheImmediatelly ) {
    // it's not completely correct. TODO: investigate how to implement it when set wakeup interval customizable
    wakeupInterval = normalizeWakeUpInterval( wakeupInterval );

    ZNET_LOGD( "setWakeUpInterval   min  %d, max  %d, interval  %d\n",
               m_minWakeUpInterval,
               m_maxWakeUpInterval,
               wakeupInterval );

    if( changeCacheImmediatelly ) {
        m_wakeupInterval = wakeupInterval;
    }

    return ZNET_ERR_OK == znet_node_cmd_wakeup_interval_set( getNodeId(), getChannelId(), wakeupInterval );
}

void ZCc0x84WakeUp::setWakeUpCapabilities( uint32_t minInterval, uint32_t maxInterval, uint32_t step ) {
    m_hasCapabilities = true;
    m_maxWakeUpInterval = maxInterval;
    m_minWakeUpInterval = minInterval;
    m_step = step;
}

bool ZCc0x84WakeUp::storeData( IFile& file ) const {
    bool result = true;
    result = result && file.write( &m_minWakeUpInterval, sizeof( m_minWakeUpInterval ) );
    result = result && file.write( &m_maxWakeUpInterval, sizeof( m_maxWakeUpInterval ) );
    result = result && file.write( &m_step, sizeof( m_step ) );
    result = result && file.write( &m_hasCapabilities, sizeof( m_hasCapabilities ) );

    // v12
    result = result && file.write( &m_wakeupInterval, sizeof( m_wakeupInterval ) );
    return result;
}

bool ZCc0x84WakeUp::restoreData( IFile& file, int version ) {
    bool result = true;
    result = result && file.read( &m_minWakeUpInterval, sizeof( m_minWakeUpInterval ) );
    result = result && file.read( &m_maxWakeUpInterval, sizeof( m_maxWakeUpInterval ) );
    result = result && file.read( &m_step, sizeof( m_step ) );
    result = result && file.read( &m_hasCapabilities, sizeof( m_hasCapabilities ) );

    if( version >= majorFormatVersion_v12 ) {
        result = result && file.read( &m_wakeupInterval, sizeof( m_wakeupInterval ) );
    }

    return result;
}

bool ZCc0x84WakeUp::hasCapabilities() const {
    return m_hasCapabilities;
}

uint32_t ZCc0x84WakeUp::getMaxWakeUpInterval() const {
    return m_maxWakeUpInterval;
}

uint32_t ZCc0x84WakeUp::getMinWakeUpInterval() const {
    return m_minWakeUpInterval;
}

uint32_t ZCc0x84WakeUp::getStepWakeUpInterval() const {
    return m_step;
}

void ZCc0x84WakeUp::onIntervalReceived( uint32_t interval, znet_node_id_t targetNodeId ) {
    const auto selfNodeId = znet_get_self_node_id();
    if( targetNodeId != selfNodeId ) {
        ZNET_LOGW( "%s target node (%u) is not self node (%u)\n", __func__, targetNodeId, selfNodeId );
    }
    m_wakeupInterval = targetNodeId == selfNodeId ? interval : 0;
    getParentNode().setWakeUpInterval( m_wakeupInterval );
}

uint32_t ZCc0x84WakeUp::getInterval() const {
    return m_wakeupInterval;
}

}  // namespace znet
