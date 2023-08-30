#pragma once

#include <znet/znet_command_class.hpp>

#include <functional>

namespace znet {

class ZCc0x81Clock final : public ZClass {
public:
    using ID = ZClassID<ZNET_COMMAND_CLASS_CLOCK>;
    using ZClass::ZClass;

public:
    bool requestTime( std::function<void( const znet_clock_value_t& )> onSuccess,
                      std::function<void( RequestFailType )> onFail,
                      WeakJobContext context );

    bool setTime( const znet_clock_value_t& clockValue );

    ZNET_DEFINE_ACCEPT()
};

}  // namespace znet
