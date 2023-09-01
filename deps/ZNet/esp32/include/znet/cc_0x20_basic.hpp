#pragma once

#include <znet/znet_command_class.hpp>

namespace znet {

class ZCc0x20Basic final : public ZClass {
public:
    using ID = ZClassID<ZNET_COMMAND_CLASS_BASIC>;

    ZCc0x20Basic( ZChannel& parent, znet_command_class_version_t version = 1 );

    bool requestValue( const RequestValue& request ) override;

    bool interview( InterviewCallback callback, WeakJobContext context ) override;
    bool storeData( IFile& file ) const override;
    bool restoreData( IFile& file, int version ) override;

    bool isSupported() const;

    ZNET_DEFINE_ACCEPT()

protected:
    bool setValue( const uint8_t value ) override;

    bool setValueMulticast( const uint8_t nodeQuantity,
                            const znet_node_id_t* listNodeId,
                            const uint8_t value,
                            bool sendSingleCasts ) override;

private:
    bool m_isSupported;
};

}  // namespace znet
