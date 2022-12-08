#pragma once

#include <znet/znet_command_class.hpp>
#include <znet/znet_door_condition.hpp>

namespace znet {

class ZCc0x62DoorLock final : public ZClass {
public:
    using ID = ZClassID<ZNET_COMMAND_CLASS_DOOR_LOCK>;
    using ZClass::ZClass;

    struct Capabilities {
        uint8_t supportedInsideDoorHandlesBitmask = 0;
        uint8_t supportedOutsideDoorHandlesBitmask = 0;
        bool isDoorSupported = false;
        bool isBoltSupported = false;
        bool isLatchSupported = false;
        bool isAutoRelockSupported = false;
        bool isBlockToBlockSupported = false;
        bool isTwistAssistSupported = false;
        bool isHoldAndReleaseSupported = false;
        bool areTimeOperationsSupported = false;
        std::vector<DoorLock::Mode> supportedModes;
    };

    using ConfigurationCallback =
        std::function<void( const znet_door_lock_configuration_t&, const znet_door_lock_extended_configuration_t* )>;
    using OperationCallback = std::function<void( const znet_door_lock_operation_report_t& )>;

    bool requestValue( const RequestValue& request ) override;

    bool interview( InterviewCallback callback, WeakJobContext context ) override;

    bool requestConfiguration( ConfigurationCallback onSuccess = {},
                               OnFailCallback onFail = {},
                               WeakJobContext context = {} );

    bool requestOperation( OperationCallback onSuccess = {}, OnFailCallback onFail = {}, WeakJobContext context = {} );

    bool setConfiguration( const znet_door_lock_configuration_t& configuration,
                           const znet_door_lock_extended_configuration_t* extendedConfiguration );

    bool setOperation( DoorLock::Mode mode );

    static DoorCondition decodeDoorCondition( uint8_t doorConditionMask );

    const Capabilities* getCapabilities() const;

    bool storeData( IFile& file ) const override;

    bool restoreData( IFile& file, int version ) override;

    ZNET_DEFINE_ACCEPT()

private:
    Capabilities m_capabilities = {};
};

}  // namespace znet
