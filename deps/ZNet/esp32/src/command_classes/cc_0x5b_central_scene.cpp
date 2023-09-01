#include "znet/cc_0x5b_central_scene.hpp"

#include "znet/znet_node.hpp"
#include "znet/znet_requests_manager.hpp"

extern "C" {
#include "znet_log.h"
}

#include <cassert>

namespace znet {

bool ZCc0x5BCentralScene::setValue( const uint8_t value ) {
    if( getVersion() < 3 )
        return true;

    return ZNET_ERR_OK == znet_node_cmd_central_scene_configuration_set( getNodeId(), getChannelId(), value );
}

bool ZCc0x5BCentralScene::requestValue( const RequestValue& request ) {
    const bool result =
        requestValueImpl( *this, CentralSceneConfigurationRequest( getNodeId(), getChannelId() ), request );

    if( !result ) {
        ZNET_LOGE( "central scene config get error, node %u\n", getNodeId() );
        return false;
    }

    return true;
}

bool ZCc0x5BCentralScene::interview( InterviewCallback callback, WeakJobContext context ) {
    if( !getParentNode().getAssociationsConfigurator().setupCommandClassAssociation(
            getNodeId(), ZNET_COMMAND_CLASS_CENTRAL_SCENE ) ) {
        ZNET_LOGW( "Setup central scene cc associations failed for node %d\n", getNodeId() );
    }

    return singleInterviewRequestImpl(
        *this,
        CentralSceneSupportedRequest( getNodeId(), getChannelId() ),
        context,
        [=]( const znet_central_scene_supported_report_t& report ) {
            setSceneNum( report.scenes_num );
            setSceneSize( report.data_size );

            if( report.data_size != 1 ) {
                ZNET_LOGW( "onNodeCmdCentralSceneSupported: wrong attribute size(%d)\n", report.data_size );
                return;
            }

            for( uint8_t i = 0; i < report.scenes_num; ++i ) {
                setAttribute( i + 1, report.data[report.identical ? 0 : i] );
            }

            if( getVersion() >= 3 ) {
                znet_node_cmd_central_scene_configuration_set( getNodeId(), getChannelId(), 1 );
            }
        },
        std::move( callback ) );
}

bool ZCc0x5BCentralScene::restoreData( IFile& file, int version ) {
    size_t count = 0;
    uint8_t param = 0;
    bool result = true;

    result = result && file.read( &param, sizeof( param ) );
    setSceneSize( param );
    result = result && file.read( &param, sizeof( param ) );
    setSceneNum( param );

    result = result && file.read( &count, sizeof( count ) );
    for( int i = 1; i <= count; ++i ) {
        result = result && file.read( &param, sizeof( param ) );
        setAttribute( i, param );
    }
    return result;
}

bool ZCc0x5BCentralScene::storeData( IFile& file ) const {
    bool result = true;

    uint8_t param = getSceneSize();
    result = result && file.write( &param, sizeof( param ) );
    param = getSceneNum();
    result = result && file.write( &param, sizeof( param ) );
    size_t count = getSceneNum();
    result = result && file.write( &count, sizeof( count ) );

    for( int i = 1; i <= count; ++i ) {
        param = getAttribute( i );
        result = result && file.write( &param, sizeof( param ) );
    }
    return result;
}

void ZCc0x5BCentralScene::updateSequence( uint8_t sequence ) {
    m_sequence_num = sequence;
}

uint8_t ZCc0x5BCentralScene::getSequence() const {
    return m_sequence_num;
}

void ZCc0x5BCentralScene::setAttribute( uint8_t scene_num, uint8_t attributes_mask ) {
    assert( scene_num <= m_scenes_num );
    m_keyAttributes[scene_num - 1] = attributes_mask;
}

uint8_t ZCc0x5BCentralScene::getAttribute( uint8_t scene_num ) const {
    assert( scene_num <= m_scenes_num );
    return m_keyAttributes.at( scene_num - 1 );
}

void ZCc0x5BCentralScene::setSceneSize( uint8_t scene_size ) {
    m_scene_size = scene_size;
}

void ZCc0x5BCentralScene::setSceneNum( uint8_t scenes_num ) {
    m_scenes_num = scenes_num;
    m_keyAttributes.clear();
    m_keyAttributes.resize( m_scenes_num, 0 );
}

uint8_t ZCc0x5BCentralScene::getSceneSize() const {
    return m_scene_size;
}

uint8_t ZCc0x5BCentralScene::getSceneNum() const {
    return m_scenes_num;
}

}  // namespace znet
