#pragma once

#include <znet/znet_command_class.hpp>
#include <znet/znet_job.hpp>

namespace znet {

class ZCc0x79SoundSwitch final : public ZClass {
public:
    struct ToneInfo {
        uint16_t duration;
        std::string name;
    };

    using ID = ZClassID<ZNET_COMMAND_CLASS_SOUND_SWITCH>;

    using ZClass::ZClass;

    bool interview( InterviewCallback callback, WeakJobContext context ) override;

    bool storeData( IFile& file ) const override;
    bool restoreData( IFile& file, int version ) override;

    bool requestValue( const RequestValue& request ) override;

    bool setConfiguration( uint8_t volume, znet_cmd_sound_switch_tone_id_t defaultToneId ) const;
    bool requestConfiguration( std::function<void( const znet_cmd_sound_switch_configuration_t& )> onSuccess = {},
                               OnFailCallback onFail = {},
                               WeakJobContext context = {} );

    bool setTonePlay( znet_cmd_sound_switch_tone_id_t toneId, uint8_t volume ) const;
    bool requestTonePlay( std::function<void( const znet_cmd_sound_switch_tone_play_t& tonePlayInfo )> onSuccess = {},
                          OnFailCallback onFail = {},
                          WeakJobContext context = {} );

    ZNET_DEFINE_ACCEPT()

    znet_cmd_sound_switch_tones_number_t getTonesNumber() const;
    uint16_t getToneDuration( znet_cmd_sound_switch_tone_id_t toneId ) const;
    const std::string& getToneName( znet_cmd_sound_switch_tone_id_t toneId ) const;

    const std::vector<ToneInfo>& getToneInfos() const;

private:
    void requestNextToneInfo( InterviewCallbackPtr callbackPtr,
                              WeakJobContext jobContext,
                              znet_cmd_sound_switch_tone_id_t toneId = ZNET_CMD_SOUND_SWITCH_TONE_ID_MIN );

    const ToneInfo& getToneInfo( znet_cmd_sound_switch_tone_id_t toneId ) const;
    ToneInfo& getToneInfo( znet_cmd_sound_switch_tone_id_t toneId );

private:
    std::vector<ToneInfo> m_toneInfos;
};

}  // namespace znet
