#pragma once

#include <znet/znet_command_class.hpp>

namespace znet {

class ZCc0x80Battery final : public ZClass {
public:
    using ID = ZClassID<ZNET_COMMAND_CLASS_BATTERY>;
    using ZClass::ZClass;

    bool requestValue( const RequestValue& request ) override;

    bool storeData( IFile& file ) const override;
    bool restoreData( IFile& file, int version ) override;

    void setForcedRequest( bool needForcedRequest );
    bool hasForcedRequest();

    ZNET_DEFINE_ACCEPT()

private:
    bool m_needForcedRequest = false;
};

}  // namespace znet
