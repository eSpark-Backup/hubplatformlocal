#pragma once

#include <znet/znet_command_class.hpp>
#include <znet/enums/user_code.hpp>

#include <unordered_set>

namespace znet {

class ZCc0x63UserCode final : public ZClass {
public:
    using UserIds = std::unordered_set<znet_supported_users_t>;
    using ID = ZClassID<ZNET_COMMAND_CLASS_USER_CODE>;

    struct ExtraSettings {
        static const size_t dontStopPinCodesScanning = 0;

        bool isExtendedUserCodesRequestRestricted = false;
        size_t unsetPinCodesCountToStop = dontStopPinCodesScanning;
        // TODO
    };

public:
    struct Capabilities {
        bool isCheckSumSupported = false;
        bool isMasterCodeSupported = false;
        bool isMasterCodeDeactivationSupported = false;
        bool isMultipleUserCodeReportSupported = false;
        bool isMultipleUserCodeSetSupported = false;
        std::vector<char> supportedKeys;
        std::vector<UserCode::KeypadMode> supportedKeypadModes;
        std::vector<UserCode::UserIDStatus> supportedUserIdStatuses;
    };

    class UserIdsCache {
    public:
        UserIdsCache( ZCc0x63UserCode& cc );
        UserIdsCache( const UserIdsCache& ) = delete;
        UserIdsCache operator=( const UserIdsCache& ) = delete;

        void onUserCodeUpdated( znet_supported_users_t userId, UserCode::UserIDStatus idStatus );
        const UserIds& getUserIds() const;

    private:
        ZCc0x63UserCode& m_userCodeCC;
        UserIds m_userIds;
    };

    ZCc0x63UserCode( ZChannel& parentChannel, const znet_command_class_version_t version = 1 );

    bool interview( InterviewCallback callback, WeakJobContext context ) override;

    void setUserCode( znet_supported_users_t userId, uint8_t userIdStatus, const std::string& userCode );

    void setMasterCode( const std::string& userCode );

    void setKeypadMode( UserCode::KeypadMode keypadMode );

    bool requestValue( const RequestValue& request ) override;

    void setSupportedUsersCount( znet_supported_users_t supportedUsersCount );
    znet_supported_users_t getSupportedUsersCount() const;

    void setExtendedUserCodesRequestRestricted( bool restricted );

    void setUnsetPinCodesCountToStop( size_t count );

    bool canRequestExtendedUserCodes() const;

    bool requestMasterCode( std::function<void( const char*, uint8_t )> onSuccess = {},
                            std::function<void( RequestFailType )> onFail = {},
                            WeakJobContext context = {} ) const;

    bool requestChecksum( std::function<void( znet_user_code_checksum_t )> onSuccess = {},
                          std::function<void( RequestFailType )> onFail = {},
                          WeakJobContext context = {} ) const;

    bool requestKeypadMode( std::function<void( znet_user_code_keypad_mode_t )> onSuccess = {},
                            std::function<void( RequestFailType )> onFail = {},
                            WeakJobContext context = {} ) const;

    bool requestAllUserCodes( RequestValue::Callback cb = {}, WeakJobContext jobContext = {} );

    UserIdsCache& getUserIdsCache();

    const Capabilities* getCapabilities() const;

    bool storeData( IFile& file ) const override;
    bool restoreData( IFile& file, int version ) override;

    ZNET_DEFINE_ACCEPT()

private:
    bool requestAllUserCodesImpl( RequestValue::CallbackPtr callbackPtr, WeakJobContext jobContext );

    void setCapabilitiesFromReport( const znet_user_code_capabilities_report_t& report );

    void requestNextUserCode( RequestValue::CallbackPtr callbackPtr,
                              WeakJobContext jobContext,
                              size_t pollingIndex,
                              znet_supported_users_t user = 1 );

private:
    UserIdsCache m_userIdsCache;
    znet_supported_users_t m_supportedUsersCount = 0;
    Capabilities m_capabilities = {};
    size_t m_pollingIndex = 0;
    size_t m_unsetPinCodesCount = 0;
    ExtraSettings m_extraSettings;

    UserCode::UserPollStatus m_status = UserCode::UserPollStatus::Idle;
};

}  // namespace znet
