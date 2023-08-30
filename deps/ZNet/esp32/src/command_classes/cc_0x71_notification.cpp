#include "znet/cc_0x71_notification.hpp"
#include "znet/znet_utils.hpp"
#include "znet_lib_cc_codes.h"
#include "znet/znet_node.hpp"
#include "znet/znet_requests_manager.hpp"

extern "C" {
#include "znet_log.h"
}

#include <algorithm>
#include <cassert>

namespace znet {

namespace {
const znet_notification_report_t& handleNotificationReport( const znet_notification_report_t& report,
                                                            bool& isRequested ) {
    isRequested = true;
    return report;
};

}  // namespace

bool ZCc0x71Notification::interview( InterviewCallback callback, WeakJobContext context ) {
    ZNET_LOGI( "Notification class interview started\n" );
    if( getVersion() <= 1 ) {
        m_v1_support = true;
        setInterviewDone( true );
        return false;
    }

    auto callbackPtr = std::make_shared<InterviewCallback>( std::move( callback ) );

    return getParentNode().getRequestsManager().doRequest(
        NotificationSupportedRequest( getNodeId(), getChannelId() ),
        context,
        [=]( const znet_notification_supported_report_t& value ) {
            m_v1_support = value.v1_alarm != 0;

            m_supportedNotification.clear();

            m_supportedNotification.reserve( utils::getBitsCount( value.bit_mask, value.bit_mask_count ) );
            utils::forEachBit( value.bit_mask, value.bit_mask_count, [this]( size_t bitIndex ) {
                m_supportedNotification.push_back( { static_cast<NotificationType>( bitIndex ), {} } );
            } );

            requestNextEvents( callbackPtr, context );
        },
        std::bind( onRequestsFinished, callbackPtr, std::placeholders::_1 ) );
}

bool ZCc0x71Notification::requestValue( const RequestValue& request ) {
    return m_doRequestValue && getVersion() > 1 && !m_supportedNotification.empty() &&
           requestNextValue(
               request.resultCallback ? std::make_shared<RequestValue::Callback>( request.resultCallback ) : nullptr,
               request.jobContext );
}

void ZCc0x71Notification::setShouldRequestValue( bool value ) {
    m_doRequestValue = value;
}

void ZCc0x71Notification::requestNextEvents( InterviewCallbackPtr callbackPtr, WeakJobContext context, uint8_t index ) {
    assert( getVersion() >= 2 );

    if( index >= m_supportedNotification.size() ) {
        setInterviewDone( true );
        onRequestsFinished( callbackPtr, {} );
        return;
    }

    const auto type = static_cast<uint8_t>( m_supportedNotification[index].m_type );

    if( getVersion() == 2 ) {
        getParentNode().getRequestsManager().doRequest(
            NotificationV2Request( getNodeId(), getChannelId(), 0, type, 0, 2 ),
            context,
            [=]( const znet_notification_report_t& report, bool& isRequested ) {
                handleNotificationReport( report, isRequested );
                m_supportedNotification[index].m_events = { report.notification_event };
                requestNextEvents( callbackPtr, context, index + 1 );
            },
            std::bind( onRequestsFinished, callbackPtr, std::placeholders::_1 ),
            DefaultPostponedRequestDelay );
    }
    else {
        getParentNode().getRequestsManager().doRequest(
            NotificationEventSupportedRequest( getNodeId(), getChannelId(), type ),
            context,
            [=]( const uint8_t* bitmask, uint8_t bitmaskSize ) {
                auto& events = m_supportedNotification[index].m_events;
                events.clear();
                events.reserve( utils::getBitsCount( bitmask, bitmaskSize ) );
                utils::forEachBit( bitmask, bitmaskSize, [&events]( uint8_t bitIndex ) {
                    events.push_back( static_cast<NotificationEvent>( bitIndex ) );
                } );

                requestNextEvents( callbackPtr, context, index + 1 );
            },
            std::bind( onRequestsFinished, callbackPtr, std::placeholders::_1 ),
            DefaultPostponedRequestDelay );
    }
}

bool ZCc0x71Notification::requestNextValue( RequestValue::CallbackPtr callbackPtr,
                                            WeakJobContext context,
                                            uint8_t typeIndex ) {
    if( typeIndex >= m_supportedNotification.size() ) {
        onRequestsFinished( callbackPtr, {} );
        return true;
    }

    const auto& notificationInfo = m_supportedNotification[typeIndex];

    return requestNotificationValue( notificationInfo.m_type,
                                     0,
                                     [=]( const znet_notification_report_t& ) {
                                         if( !requestNextValue( callbackPtr, context, typeIndex + 1 ) ) {
                                             onRequestsFinished( callbackPtr, RequestFailType::SendingError );
                                         }
                                     },
                                     std::bind( onRequestsFinished, callbackPtr, std::placeholders::_1 ),
                                     context );
}

bool ZCc0x71Notification::isV1Supported() const {
    return m_v1_support;
}

const std::vector<Notification>& ZCc0x71Notification::getSupportedNotifications() const {
    return m_supportedNotification;
}

bool ZCc0x71Notification::requestV1AlarmValue( uint8_t v1AlarmType,
                                               OnNotificationRequestSuccess onSuccess,
                                               OnFailCallback onFail,
                                               WeakJobContext context ) {
    return m_v1_support && getParentNode().getRequestsManager().doRequest(
                               NotificationV1Request( getNodeId(), getChannelId(), v1AlarmType, 0, 0, 1 ),
                               context,
                               // TODO: move onSuccess to lambda on c++14
                               [onSuccess]( const znet_notification_report_t& report, bool& isRequested ) {
                                   if( onSuccess ) {
                                       onSuccess( handleNotificationReport( report, isRequested ) );
                                   }
                               },
                               std::move( onFail ) );
}

bool ZCc0x71Notification::requestNotificationValue( NotificationType notificationType,
                                                    NotificationEvent notificationEvent,
                                                    OnNotificationRequestSuccess onSuccess,
                                                    OnFailCallback onFail,
                                                    WeakJobContext context ) {
    if( getVersion() < 2 ) {
        return false;
    }

    const auto askingVersion = notificationEvent ?
                                   getVersion() :
                                   2;  // use second version if event is unspecified. Zwave doc strongly requires to
                                       // send status only for alarm V2. Devices report 0xFE event if we send V3 or more

    return getParentNode().getRequestsManager().doRequest(
        NotificationV2Request( getNodeId(),
                               getChannelId(),
                               0,
                               static_cast<uint8_t>( notificationType ),
                               notificationEvent,
                               askingVersion ),
        context,
        std::bind( std::move( onSuccess ),
                   std::bind( handleNotificationReport, std::placeholders::_1, std::placeholders::_2 ) ),
        std::move( onFail ) );
}

bool ZCc0x71Notification::storeData( IFile& file ) const {
    const auto notificationsCount = static_cast<std::uint8_t>( m_supportedNotification.size() );

    bool result = file.write( &m_v1_support, sizeof( m_v1_support ) );
    result = result && file.write( &m_doRequestValue, sizeof( m_doRequestValue ) );
    result = result && file.write( &notificationsCount, sizeof( notificationsCount ) );

    for( const auto& notification : m_supportedNotification ) {
        const auto eventsSize = static_cast<std::uint8_t>( notification.m_events.size() );
        result = result && file.write( &notification.m_type, sizeof( notification.m_type ) );
        result = result && file.write( &eventsSize, sizeof( eventsSize ) );
        if( eventsSize > 0 ) {
            result = result && file.write( notification.m_events.data(), eventsSize );
        }
    }
    return result;
}

bool ZCc0x71Notification::restoreData( IFile& file, int version ) {
    bool v1Support;
    uint8_t notificationCount;

    if( !file.read( &v1Support, sizeof( v1Support ) ) ) {
        return false;
    }

    if( version >= majorFormatVersion_v15 ) {
        if( !file.read( &m_doRequestValue, sizeof( m_doRequestValue ) ) ) {
            return false;
        }
    }
    else {
        m_doRequestValue = true;
    }

    if( !file.read( &notificationCount, sizeof( notificationCount ) ) ) {
        return false;
    }

    std::vector<Notification> notifications;
    notifications.reserve( notificationCount );

    for( uint8_t i = 0u; i < notificationCount; ++i ) {
        uint8_t notification, eventsSize;
        if( !file.read( &notification, sizeof( notification ) ) || !file.read( &eventsSize, sizeof( eventsSize ) ) ) {
            return false;
        }

        std::vector<NotificationEvent> events;
        events.resize( eventsSize, NotificationEvent( 0 ) );
        if( eventsSize > 0 && !file.read( &events[0], eventsSize ) ) {
            return false;
        }

        notifications.push_back( { static_cast<NotificationType>( notification ), std::move( events ) } );
    }

    m_v1_support = v1Support;
    m_supportedNotification = std::move( notifications );
    return true;
}

}  // namespace znet
