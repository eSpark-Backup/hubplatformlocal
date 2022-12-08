#pragma once

#include <znet/cc_anti_theft_base.hpp>

namespace znet {

class ZCc0x7EAntiTheftUnlock final : public ZCcAntiTheftBase {
public:
    using ID = ZClassID<ZNET_COMMAND_CLASS_ANTITHEFT_UNLOCK>;
    using ZCcAntiTheftBase::ZCcAntiTheftBase;

    bool unlock( const std::vector<uint8_t>& magicCode );

    bool unlock( const std::string& hexMagicCode );

    bool requestValue( const RequestValue& request ) override;

    bool interview( InterviewCallback callback, WeakJobContext context ) override;

    ZNET_DEFINE_ACCEPT()
};
}  // namespace znet
