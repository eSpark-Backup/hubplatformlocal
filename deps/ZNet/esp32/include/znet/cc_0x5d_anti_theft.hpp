#pragma once

#include <znet/cc_anti_theft_base.hpp>

namespace znet {

class ZCc0x5DAntiTheft final : public ZCcAntiTheftBase {
public:
    using ID = ZClassID<ZNET_COMMAND_CLASS_ANTITHEFT>;
    using ZCcAntiTheftBase::ZCcAntiTheftBase;

    bool lock( const std::vector<uint8_t>& magicCode,
               const std::vector<uint8_t>& hint,
               uint16_t manufacturerId,
               uint16_t zwaveLockingId );

    bool lock( const std::string& hexMagicCode,
               const std::string& hexHint,
               uint16_t manufacturerId,
               uint16_t zwaveLockingId );

    bool unlock( const std::vector<uint8_t>& magicCode );

    bool unlock( const std::string& hexMagicCode );

    bool requestValue( const RequestValue& request ) override;

    bool interview( InterviewCallback callback, WeakJobContext context ) override;

    ZNET_DEFINE_ACCEPT()
};
}  // namespace znet
