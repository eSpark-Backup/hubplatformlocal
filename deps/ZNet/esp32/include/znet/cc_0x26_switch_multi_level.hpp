#pragma once
#include <znet/znet_command_class.hpp>
#include <znet/enums/value_change_command.hpp>

namespace znet {

using Seconds = std::chrono::seconds;
using SecondsOpt = utils::optional<Seconds>;

class ZCc0x26SwitchMultiLevel : public ZClass {
    static const int DurationMsToSec;
    static const int DurationMsToMin;

public:
    using ID = ZClassID<ZNET_COMMAND_CLASS_SWITCH_MULTILEVEL>;
    using ZClass::ZClass;

    bool requestValue( const RequestValue& request ) override;

    bool setLevelChange( ValueChangeCommand command, const SecondsOpt& duration = {} );
    bool startLevelChange( bool downDirection,
                           const uint8_t startValue = ZNET_CMD_MULTILEVEL_SWITCH_VALUE_UNKNOWN,
                           const SecondsOpt& seconds = {} );
    bool stopLevelChange();

    bool interview( InterviewCallback callback, WeakJobContext context ) override;
    bool storeData( IFile& file ) const override;
    bool restoreData( IFile& file, int version ) override;

    uint8_t getPrimaryType() const;
    uint8_t getSecondaryType() const;

    uint8_t getTargetLevel();

    ZNET_DEFINE_ACCEPT()

protected:
    bool setValue( const uint8_t value ) override;
    bool setValueMulticast( const uint8_t nodeQuantity,
                            const znet_node_id_t* listNodeId,
                            const uint8_t value,
                            bool sendSingleCasts ) override;

private:
    uint8_t m_target_level = 0;

    uint8_t m_primaryType = ZNET_CMD_MULTILEVEL_SWITCH_VALUE_UNKNOWN;
    uint8_t m_secondaryType = ZNET_CMD_MULTILEVEL_SWITCH_VALUE_UNKNOWN;
};

}  // namespace znet
