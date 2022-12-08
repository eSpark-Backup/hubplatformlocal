#include "znet/cc_0x87_indicator.hpp"

#include "znet/znet_node.hpp"
#include "znet/znet_requests_manager.hpp"

#include "znet/znet_utils.hpp"

extern "C" {
#include "znet_log.h"
}

#include <string.h>
#include <cassert>
#include <algorithm>

namespace znet {

bool ZCc0x87Indicator::interview( InterviewCallback callback, WeakJobContext context ) {
    ZNET_LOGD( "indicator interview started, ver = %d\n", getVersion() );

    m_indicators.clear();

    if( getVersion() == 1 ) {
        m_indicators.push_back( { 1, std::vector<uint8_t>() } );
        return singleInterviewRequestImpl( *this,
                                           IndicatorRequestV1( getNodeId(), getChannelId() ),
                                           context,
                                           []( const znet_indicator_report_t& report ) {
                                               if( report.indicator_objects_count > 0 ) {
                                                   ZNET_LOGW(
                                                       "Indicator version is 1 but indicator_objects_count = %u\n",
                                                       report.indicator_objects_count );
                                               }
                                               // Nothing to do
                                           },
                                           std::move( callback ) );
    }
    else {
        auto callbackPtr = std::make_shared<InterviewCallback>( std::move( callback ) );

        return getParentNode().getRequestsManager().doRequest(
            IndicatorSupportedRequestWithUnspecifiedId( getNodeId(), getChannelId(), 0 ),
            context,
            [=]( const znet_indicator_supported_report_t& report ) {
                onIndicatorSupported( callbackPtr, context, report );
            },
            std::bind( onRequestsFinished, callbackPtr, std::placeholders::_1 ) );
    }
}

void ZCc0x87Indicator::sendIdentifyCommand() const {
    if( getVersion() >= 3 ) {
        znet_node_cmd_indicator_identify( getNodeId(), getChannelId() );
    }
}

void ZCc0x87Indicator::onIndicatorSupported( InterviewCallbackPtr callbackPtr,
                                             WeakJobContext context,
                                             const znet_indicator_supported_report_t& report ) {
    IndicatorData data;
    data.id = report.indicator_id;

    utils::forEachBit( report.bit_mask + 1, report.supported_mask_len, [&data]( uint8_t index ) {
        data.supportedProperties.push_back( index + 1 );
    } );

    m_indicators.push_back( std::move( data ) );

    if( report.next_indicator_id != 0 ) {
        getParentNode().getRequestsManager().doRequest(
            IndicatorSupportedRequest( getNodeId(), getChannelId(), report.next_indicator_id ),
            context,
            [=]( const znet_indicator_supported_report_t& report ) {
                onIndicatorSupported( callbackPtr, context, report );
            },
            std::bind( onRequestsFinished, callbackPtr, std::placeholders::_1 ),
            DefaultPostponedRequestDelay );
    }
    else {
        assert( !m_indicators.empty() );
        getParentNode().getRequestsManager().doRequest(
            IndicatorRequest( getNodeId(), getChannelId(), m_indicators[0].id ),
            context,
            [=]( const znet_indicator_report_t& report ) { requestNextIndicatorValue( callbackPtr, context, 1 ); },
            std::bind( onRequestsFinished, callbackPtr, std::placeholders::_1 ),
            DefaultPostponedRequestDelay );
    }
}

void ZCc0x87Indicator::requestNextIndicatorValue( InterviewCallbackPtr callbackPtr,
                                                  WeakJobContext context,
                                                  uint8_t nextIndex ) {
    if( nextIndex >= m_indicators.size() ) {
        setInterviewDone( true );
        onRequestsFinished( callbackPtr, {} );
        return;
    }

    getParentNode().getRequestsManager().doRequest(
        IndicatorRequest( getNodeId(), getChannelId(), m_indicators[nextIndex].id ),
        context,
        [=]( const znet_indicator_report_t& report ) {
            requestNextIndicatorValue( callbackPtr, context, nextIndex + 1 );
        },
        std::bind( onRequestsFinished, callbackPtr, std::placeholders::_1 ),
        DefaultPostponedRequestDelay );
}

bool ZCc0x87Indicator::storeData( IFile& file ) const {
    bool result = true;

    const uint8_t numIndicators = static_cast<uint8_t>( m_indicators.size() );
    result = result && file.write( &numIndicators, sizeof( numIndicators ) );

    for( auto indicator : m_indicators ) {
        result = result && file.write( &indicator.id, sizeof( indicator.id ) );
        const uint8_t numProperties = static_cast<uint8_t>( indicator.supportedProperties.size() );
        result = result && file.write( &numProperties, sizeof( numProperties ) );
        for( auto property : indicator.supportedProperties ) {
            result = result && file.write( &property, sizeof( property ) );
        }
    }

    assert( result );

    return result;
}

bool ZCc0x87Indicator::restoreData( IFile& file, int version ) {
    bool result = true;

    uint8_t numIndicators = 0;
    result = result && file.read( &numIndicators, sizeof( numIndicators ) );

    m_indicators.reserve( numIndicators );
    for( size_t n = 0; n < numIndicators; n++ ) {
        IndicatorData data;
        result = result && file.read( &data.id, sizeof( data.id ) );

        uint8_t numProperties = 0;
        result = result && file.read( &numProperties, sizeof( numProperties ) );
        data.supportedProperties.reserve( numProperties );

        for( size_t m = 0; m < numProperties; m++ ) {
            uint8_t property = 0;
            result = result && file.read( &property, sizeof( property ) );
            data.supportedProperties.push_back( 0 );
        }
        m_indicators.push_back( std::move( data ) );
    }

    assert( result );

    return result;
}

}  // namespace znet
