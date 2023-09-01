#pragma once

#include <znet/znet_command_class.hpp>
#include <znet/znet_notification.hpp>
#include <vector>
#include <chrono>

namespace znet {

class ZCc0x71Notification final : public ZClass {
public:
    using ID = ZClassID<ZNET_COMMAND_CLASS_NOTIFICATION>;
    using ZClass::ZClass;

    using OnNotificationRequestSuccess = std::function<void( const znet_notification_report_t& )>;

    bool interview( InterviewCallback callback, WeakJobContext context ) override;

    bool requestValue( const RequestValue& request ) override;

    void setShouldRequestValue( bool value );

    bool isV1Supported() const;

    const std::vector<Notification>& getSupportedNotifications() const;

    bool storeData( IFile& file ) const override;
    bool restoreData( IFile& file, int version ) override;

    bool requestV1AlarmValue( uint8_t v1AlarmType,
                              OnNotificationRequestSuccess onSuccess = {},
                              OnFailCallback onFail = {},
                              WeakJobContext context = {} );

    bool requestNotificationValue( NotificationType notificationType,
                                   NotificationEvent notificationEvent = {},
                                   OnNotificationRequestSuccess onSuccess = {},
                                   OnFailCallback onFail = {},
                                   WeakJobContext context = {} );

private:
    void requestNextEvents( InterviewCallbackPtr callbackPtr, WeakJobContext context, uint8_t index = 0 );

    bool requestNextValue( RequestValue::CallbackPtr callbackPtr, WeakJobContext context, uint8_t typeIndex = 0 );

private:
    bool m_v1_support = false;
    bool m_doRequestValue = true;
    std::vector<Notification> m_supportedNotification;

    ZNET_DEFINE_ACCEPT()
};

}  // namespace znet
