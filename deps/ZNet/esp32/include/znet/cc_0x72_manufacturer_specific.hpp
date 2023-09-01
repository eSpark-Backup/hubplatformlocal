#pragma once

#include <znet/znet_command_class.hpp>

namespace znet {

class ZCc0x72ManufacturerSpecific final : public ZClass {
public:
    using ID = ZClassID<ZNET_COMMAND_CLASS_MANUFACTURER_SPECIFIC>;
    using ZClass::ZClass;

    bool requestValue( const RequestValue& request ) override;

    bool requestManufacturerSpecific( std::function<void( const znet_manufacturer_specific_report_t& )> onSuccess,
                                      OnFailCallback onFail,
                                      WeakJobContext context );

    ZNET_DEFINE_ACCEPT()
};

}  // namespace znet
