#pragma once

#include <znet/znet_command_class.hpp>

namespace znet {

class ZCc0x8BTimeParameters final : public ZClass {
public:
    using ID = ZClassID<ZNET_COMMAND_CLASS_TIME_PARAMETERS>;
    using ZClass::ZClass;

public:
    bool setValue( const znet_date_t& dateUtc, const znet_time_t& timeUtc );

    bool requestValue( const RequestValue& request ) override;

    bool requestTimeParameters( std::function<void( const znet_date_time_t& )> onSuccess,
                                std::function<void( RequestFailType )> onFail,
                                WeakJobContext context ) const;

    ZNET_DEFINE_ACCEPT()
};

}  // namespace znet
