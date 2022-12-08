#include "znet/cc_0x79_sound_switch.hpp"
#include "znet/znet_node.hpp"
#include "znet/znet_requests_manager.hpp"

#include "znet/znet_request_value.hpp"

extern "C" {
#include "znet_log.h"
}

namespace znet {

bool ZCc0x79SoundSwitch::interview( InterviewCallback callback, WeakJobContext context ) {
    auto callbackPtr = std::make_shared<InterviewCallback>( std::move( callback ) );

    return getParentNode().getRequestsManager().doRequest(
        SoundSwitchTonesNumberRequest( getNodeId(), getChannelId() ),
        context,
        [=]( znet_cmd_sound_switch_tones_number_t tonesNumber ) {
            m_toneInfos.resize( tonesNumber );
            requestNextToneInfo( callbackPtr, context );
        },
        std::bind( onRequestsFinished, callbackPtr, std::placeholders::_1 ) );
}

bool ZCc0x79SoundSwitch::storeData( IFile& file ) const {
    const uint8_t tonesNumber = static_cast<uint8_t>( m_toneInfos.size() );
    if( !file.write( &tonesNumber, sizeof( uint8_t ) ) ) {
        return false;
    }

    for( const ToneInfo& toneInfo : m_toneInfos ) {
        const uint8_t nameLength = static_cast<uint8_t>( toneInfo.name.size() );

        const bool result = file.write( &toneInfo.duration, sizeof( uint16_t ) ) &&
                            file.write( &nameLength, sizeof( uint8_t ) ) &&
                            file.write( toneInfo.name.data(), sizeof( std::string::value_type ) * nameLength );

        if( !result ) {
            return false;
        }
    }

    return true;
}

bool ZCc0x79SoundSwitch::restoreData( IFile& file, int version ) {
    uint8_t tonesNumber;
    if( !file.read( &tonesNumber, sizeof( uint8_t ) ) ) {
        return false;
    }

    m_toneInfos.clear();
    m_toneInfos.reserve( tonesNumber );

    std::string::value_type buffer[0xFF];
    uint16_t duration;
    uint8_t nameLength;
    for( znet_cmd_sound_switch_tones_number_t i = 0; i != tonesNumber; ++i ) {
        const bool result = file.read( &duration, sizeof( uint16_t ) ) && file.read( &nameLength, sizeof( uint8_t ) ) &&
                            file.read( buffer, sizeof( std::string::value_type ) * nameLength );

        if( !result ) {
            return false;
        }

        m_toneInfos.push_back( { duration, std::string( buffer, nameLength ) } );
    }

    return true;
}

bool ZCc0x79SoundSwitch::requestValue( const RequestValue& request ) {
    auto context = request.jobContext;

    auto callbackPtr =
        request.resultCallback ? std::make_shared<RequestValue::Callback>( request.resultCallback ) : nullptr;

    return !m_toneInfos.empty() &&
           requestConfiguration(
               [=]( const znet_cmd_sound_switch_configuration_t& ) {
                   if( !requestValueImpl(
                           *this, SoundSwitchTonePlayRequest( getNodeId(), getChannelId() ), callbackPtr, context ) ) {
                       onRequestsFinished( callbackPtr, RequestFailType::SendingError );
                   }
               },
               std::bind( onRequestsFinished, callbackPtr, std::placeholders::_1 ),
               context );
}

bool ZCc0x79SoundSwitch::setConfiguration( uint8_t volume, znet_cmd_sound_switch_tone_id_t defaultToneId ) const {
    const znet_error_t res =
        znet_node_cmd_sound_switch_configuration_set( getNodeId(), getChannelId(), volume, defaultToneId );

    return res == ZNET_ERR_OK;
}

bool ZCc0x79SoundSwitch::requestConfiguration(
    std::function<void( const znet_cmd_sound_switch_configuration_t& )> onSuccess,
    std::function<void( RequestFailType )> onFail,
    WeakJobContext context ) {
    return getParentNode().getRequestsManager().doRequest(
        SoundSwitchConfigurationRequest( getNodeId(), getChannelId() ),
        context,
        std::move( onSuccess ),
        std::move( onFail ) );
}

bool ZCc0x79SoundSwitch::setTonePlay( znet_cmd_sound_switch_tone_id_t toneId, uint8_t volume ) const {
    return znet_node_cmd_sound_switch_tone_play_set(
               getNodeId(), getChannelId(), toneId, getVersion() == 1 ? ( toneId == 0x00 ? 0x00 : 0xFF ) : volume ) ==
           ZNET_ERR_OK;
}

bool ZCc0x79SoundSwitch::requestTonePlay(
    std::function<void( const znet_cmd_sound_switch_tone_play_t& tonePlayInfo )> onSuccess,
    OnFailCallback onFail,
    WeakJobContext context ) {
    return getParentNode().getRequestsManager().doRequest( SoundSwitchTonePlayRequest( getNodeId(), getChannelId() ),
                                                           context,
                                                           std::move( onSuccess ),
                                                           std::move( onFail ) );
}

znet_cmd_sound_switch_tones_number_t ZCc0x79SoundSwitch::getTonesNumber() const {
    return static_cast<uint8_t>( m_toneInfos.size() );
}

uint16_t ZCc0x79SoundSwitch::getToneDuration( znet_cmd_sound_switch_tone_id_t toneId ) const {
    return getToneInfo( toneId ).duration;
}

const std::string& ZCc0x79SoundSwitch::getToneName( znet_cmd_sound_switch_tone_id_t toneId ) const {
    return getToneInfo( toneId ).name;
}

void ZCc0x79SoundSwitch::requestNextToneInfo( InterviewCallbackPtr callbackPtr,
                                              WeakJobContext jobContext,
                                              znet_cmd_sound_switch_tone_id_t toneId ) {
    assert( toneId >= ZNET_CMD_SOUND_SWITCH_TONE_ID_MIN );

    if( toneId > m_toneInfos.size() ) {
        setInterviewDone( true );
        onRequestsFinished( callbackPtr, {} );
        return;
    }

    getParentNode().getRequestsManager().doRequest(
        SoundSwitchToneInfoRequest( getNodeId(), getChannelId(), toneId ),
        jobContext,
        [=]( const znet_cmd_sound_switch_tone_info_t& toneInfo ) {
            getToneInfo( toneInfo.tone_id ) = { toneInfo.tone_duration,
                                                std::string( reinterpret_cast<const char*>( toneInfo.name ),
                                                             toneInfo.name_length ) };

            requestNextToneInfo( callbackPtr, jobContext, toneId + 1 );
        },
        std::bind( onRequestsFinished, callbackPtr, std::placeholders::_1 ),
        DefaultPostponedRequestDelay );
}

const ZCc0x79SoundSwitch::ToneInfo& ZCc0x79SoundSwitch::getToneInfo( znet_cmd_sound_switch_tone_id_t toneId ) const {
    // Tone IDs start with 1
    return m_toneInfos.at( toneId - 1 );
}

ZCc0x79SoundSwitch::ToneInfo& ZCc0x79SoundSwitch::getToneInfo( znet_cmd_sound_switch_tone_id_t toneId ) {
    // Tone IDs start with 1
    return m_toneInfos.at( toneId - 1 );
}

const std::vector<ZCc0x79SoundSwitch::ToneInfo>& ZCc0x79SoundSwitch::getToneInfos() const {
    return m_toneInfos;
}

}  // namespace znet
