#pragma once
#include <znet/znet_command_class.hpp>

namespace znet {

class ZCc0x87Indicator final : public ZClass {
private:
    struct IndicatorData {
        uint8_t id;
        std::vector<uint8_t> supportedProperties;
    };

public:
    using ID = ZClassID<ZNET_COMMAND_CLASS_INDICATOR>;
    using ZClass::ZClass;

    bool interview( InterviewCallback callback, WeakJobContext context ) override;

    void sendIdentifyCommand() const;

    bool storeData( IFile& file ) const override;
    bool restoreData( IFile& file, int version ) override;

    ZNET_DEFINE_ACCEPT()

private:
    void onIndicatorSupported( InterviewCallbackPtr callbackPtr,
                               WeakJobContext context,
                               const znet_indicator_supported_report_t& report );

    void requestNextIndicatorValue( InterviewCallbackPtr callbackPtr, WeakJobContext context, uint8_t nextIndex );

private:
    std::vector<IndicatorData> m_indicators;
};

}  // namespace znet
