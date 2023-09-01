#pragma once
#include <znet/znet_command_class.hpp>

namespace znet {

class ZCc0x5BCentralScene final : public ZClass {
public:
    using ID = ZClassID<ZNET_COMMAND_CLASS_CENTRAL_SCENE>;
    using ZClass::ZClass;

    bool requestValue( const RequestValue& request ) override;
    bool interview( InterviewCallback callback, WeakJobContext context ) override;

    bool storeData( IFile& file ) const override;
    bool restoreData( IFile& file, int version ) override;

public:
    uint8_t getSequence() const;
    uint8_t getAttribute( uint8_t scene_num ) const;
    uint8_t getSceneSize() const;
    uint8_t getSceneNum() const;

    ZNET_DEFINE_ACCEPT()
protected:
    bool setValue( const uint8_t value ) override;

private:
    void updateSequence( uint8_t sequence );
    void setAttribute( uint8_t scene_num, uint8_t attributes_mask );
    void setSceneSize( uint8_t scene_size );
    void setSceneNum( uint8_t scenes_num );

private:
    bool m_slowRefresh = false;

    uint8_t m_sequence_num = 255;
    uint8_t m_scene_size = 0;
    uint8_t m_scenes_num = 0;

    std::vector<uint8_t> m_keyAttributes;
};

}  // namespace znet
