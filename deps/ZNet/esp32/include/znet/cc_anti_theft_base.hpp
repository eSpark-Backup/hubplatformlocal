#pragma once

#include <znet/znet_command_class.hpp>
#include <znet/anti_theft_info.hpp>

namespace znet {

class ZCcAntiTheftBase : public ZClass {
public:
    static const uint8_t MinMagicCodeLength = 1;
    static const uint8_t MaxMagicCodeLength = 10;

    using ZClass::ZClass;

    bool storeData( IFile& file ) const override;

    bool restoreData( IFile& file, int version ) override;

    const AntiTheftInfoOpt& getInitialLockInfo() const;

protected:
    void setInitialLockInfo( AntiTheftInfoOpt&& info );

private:
    AntiTheftInfoOpt m_initialLockInfo;
};
}  // namespace znet
