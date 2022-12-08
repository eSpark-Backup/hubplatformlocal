#include "znet/cc_0x60_multi_channel.hpp"
#include "znet/znet_requests_manager.hpp"

#include <algorithm>

extern "C" {
#include "znet_log.h"
}

namespace znet {

bool ZCc0x60MultiChannel::buildChannels( InterviewCallback callback, WeakJobContext jobContext ) {
    auto callbackPtr = std::make_shared<InterviewCallback>( std::move( callback ) );

    return getParentNode().getRequestsManager().doRequest(
        MultichannelEndpointRequest( getNodeId() ),
        jobContext,
        [=]( const znet_multichannel_endpoint_report_t& value ) {
            auto channelsCount = getParentNode().getChannelsCount();
            if( channelsCount < value.channel_ids_count + 1 ) {
                getParentNode().createAdditionalChannels( value.channel_ids_count - channelsCount + 1 );
            }

            if( getVersion() >= 4 ) {
                getParentNode().setAggregatedChannelsCount( value.agregated_channel_ids_count );
            }

            processChannelsFind( callbackPtr, jobContext );
        },
        std::bind( onRequestsFinished, callbackPtr, std::placeholders::_1 ) );
}

void ZCc0x60MultiChannel::processChannelsFind( InterviewCallbackPtr callbackPtr,
                                               WeakJobContext context,
                                               bool doRequest ) {
    getParentNode().getRequestsManager().doRequest(
        MultichannelEndpointFindRequest( getNodeId(), 0xFF, 0xFF, doRequest ),
        context,
        [=]( const znet_multichannel_endpoint_find_report_t& value ) {
            // Nothing to do with this data
            if( value.reports_to_follow != 0 ) {
                processChannelsFind( callbackPtr, context, false );
            }
            else {
                processNextChannel( callbackPtr, context );
            }
        },
        [=]( RequestFailType failType ) {
            ZNET_LOGE( "%s fail = %d\n", __func__, failType );
            if( failType == RequestFailType::Timeout ) {
                processNextChannel( callbackPtr, context );
            }
            else {
                onRequestsFinished( callbackPtr, failType );
            }
        },
        DefaultPostponedRequestDelay );
}

void ZCc0x60MultiChannel::processNextChannel( InterviewCallbackPtr callbackPtr,
                                              WeakJobContext context,
                                              znet_node_channel_id_t channelId ) {
    if( channelId >= getParentNode().getAllChannelsCount() ) {
        onRequestsFinished( callbackPtr, {} );
        return;
    }

    auto requestCapability = [=]() {
        getParentNode().getRequestsManager().doRequest(
            MultichannelEndpointCapabilityRequest( getNodeId(), channelId ),
            context,
            [=]( const znet_multichannel_capability_report_t& value ) {
                // Add only not dynamic classes
                if( !value.dynamic ) {
                    addCommandClasses( channelId, value.commands, value.commands_count );
                }

                processNextChannel( callbackPtr, context, channelId + 1 );
            },
            std::bind( onRequestsFinished, callbackPtr, std::placeholders::_1 ),
            DefaultPostponedRequestDelay );
    };

    auto requestSecurity = [=]() {
        getParentNode().getRequestsManager().doRequest( SecuritySupportedRequest( getNodeId(), channelId ),
                                                        context,
                                                        [=]( const znet_security_supported_report_t* value ) {
                                                            if( value ) {
                                                                addCommandClasses(
                                                                    channelId, value->commands, value->commands_count );
                                                            }
                                                            requestCapability();
                                                        },
                                                        std::bind( requestCapability ),
                                                        DefaultPostponedRequestDelay );
    };

    auto aggregatedChannel = [=]() {
        getParentNode().getRequestsManager().doRequest(
            MultichannelAggregatedMemberRequest( getNodeId(), channelId ),
            context,
            [=]( const znet_multichannel_aggregated_members_report_t& value ) {
                // Do nothing
                processNextChannel( callbackPtr, context, channelId + 1 );
            },
            std::bind( onRequestsFinished, callbackPtr, std::placeholders::_1 ),
            DefaultPostponedRequestDelay );
    };

    if( channelId >= getParentNode().getChannelsCount() ) {
        aggregatedChannel();
    }
    else if( getParentNode().getSecurity() == SecurityMode::Class::None ) {
        requestCapability();
    }
    else {
        requestSecurity();
    }
}

void ZCc0x60MultiChannel::addCommandClasses( znet_node_channel_id_t channelId,
                                             const znet_command_class_t* classes,
                                             uint8_t size ) {
    getParentNode().addCommandClass( channelId, ZNET_COMMAND_CLASS_BASIC );

    for( uint8_t i = 0; i < size && classes[i] != ZNET_COMMAND_CLASS_MARK; ++i ) {
        if( classes[i] > 0 ) {
            getParentNode().addCommandClass( channelId, classes[i] );
        }
    }
}

}  // namespace znet
