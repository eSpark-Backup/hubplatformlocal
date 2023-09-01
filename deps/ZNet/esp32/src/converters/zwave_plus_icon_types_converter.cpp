#include <znet/converters/zwave_plus_icon_types_converter.hpp>

#include <znet/enums/zwave_plus_icon_types.hpp>
#include <znet/converters/resources.hpp>
#include <znet/znet_utils.hpp>

extern "C" {
#include <znet_log.h>
}

namespace znet {
namespace zwave_plus_icon_types_converter {

using namespace resources::zwave_plus_icons;

namespace {

struct ZwavePlusIconTypeData {
    const ZwavePlusIconType type;
    const char* const name;
};

const ZwavePlusIconTypeData zwavePlusIconTypeData[] = {
    { ZwavePlusIconType::IconTypeGenericLightDimmerSwitch, IconTypeGenericLightDimmerSwitch },
    { ZwavePlusIconType::IconTypeSpecificLightDimmerSwitchWallLamp, IconTypeSpecificLightDimmerSwitchWallLamp },
    { ZwavePlusIconType::IconTypeSpecificLightDimmerSwitchLampPostHigh, IconTypeSpecificLightDimmerSwitchLampPostHigh },
    { ZwavePlusIconType::IconTypeSpecificLightDimmerSwitchLampPostLow, IconTypeSpecificLightDimmerSwitchLampPostLow },
    { ZwavePlusIconType::IconTypeGenericOnOffPowerSwitch, IconTypeGenericOnOffPowerSwitch },
    { ZwavePlusIconType::IconTypeSpecificOnOffPowerSwitchWallLamp, IconTypeSpecificOnOffPowerSwitchWallLamp },
    { ZwavePlusIconType::IconTypeSpecificOnOffPowerSwitchLampPostHigh, IconTypeSpecificOnOffPowerSwitchLampPostHigh },
    { ZwavePlusIconType::IconTypeSpecificOnOffPowerSwitchLampPostLow, IconTypeSpecificOnOffPowerSwitchLampPostLow },
    { ZwavePlusIconType::IconTypeSpecificLightDimmerSwitchPlugin, IconTypeSpecificLightDimmerSwitchPlugin },
    { ZwavePlusIconType::IconTypeSpecificLightDimmerSwitchWallOutlet, IconTypeSpecificLightDimmerSwitchWallOutlet },
    { ZwavePlusIconType::IconTypeSpecificLightDimmerSwitchCeilingOutlet,
      IconTypeSpecificLightDimmerSwitchCeilingOutlet },
    { ZwavePlusIconType::IconTypeGenericFanSwitch, IconTypeGenericFanSwitch },
    { ZwavePlusIconType::IconTypeGenericDimmerWallSwitch, IconTypeGenericDimmerWallSwitch },
    { ZwavePlusIconType::IconTypeSpecificDimmerWallSwitchOneButton, IconTypeSpecificDimmerWallSwitchOneButton },
    { ZwavePlusIconType::IconTypeSpecificDimmerWallSwitchTwoButtons, IconTypeSpecificDimmerWallSwitchTwoButtons },
    { ZwavePlusIconType::IconTypeSpecificDimmerWallSwitchThreeButtons, IconTypeSpecificDimmerWallSwitchThreeButtons },
    { ZwavePlusIconType::IconTypeSpecificDimmerWallSwitchFourButtons, IconTypeSpecificDimmerWallSwitchFourButtons },
    { ZwavePlusIconType::IconTypeSpecificDimmerWallSwitchOneRotary, IconTypeSpecificDimmerWallSwitchOneRotary },
    { ZwavePlusIconType::IconTypeSpecificOnOffPowerSwitchPlugin, IconTypeSpecificOnOffPowerSwitchPlugin },
    { ZwavePlusIconType::IconTypeSpecificOnOffPowerSwitchWallOutlet, IconTypeSpecificOnOffPowerSwitchWallOutlet },
    { ZwavePlusIconType::IconTypeSpecificOnOffPowerSwitchCeilingOutlet, IconTypeSpecificOnOffPowerSwitchCeilingOutlet },
    { ZwavePlusIconType::IconTypeGenericOnOffWallSwitch, IconTypeGenericOnOffWallSwitch },
    { ZwavePlusIconType::IconTypeSpecificOnOffWallSwitchOneButton, IconTypeSpecificOnOffWallSwitchOneButton },
    { ZwavePlusIconType::IconTypeSpecificOnOffWallSwitchTwoButtons, IconTypeSpecificOnOffWallSwitchTwoButtons },
    { ZwavePlusIconType::IconTypeSpecificOnOffWallSwitchThreeButtons, IconTypeSpecificOnOffWallSwitchThreeButtons },
    { ZwavePlusIconType::IconTypeSpecificOnOffWallSwitchFourButtons, IconTypeSpecificOnOffWallSwitchFourButtons },
    { ZwavePlusIconType::IconTypeSpecificOnOffWallSwitchOneRotary, IconTypeSpecificOnOffWallSwitchOneRotary },
    { ZwavePlusIconType::IconTypeGenericValveOpenClose, IconTypeGenericValveOpenClose },
    { ZwavePlusIconType::IconTypeGenericIrrigation, IconTypeGenericIrrigation },
    { ZwavePlusIconType::IconTypeSpecificLightDimmerSwitchDinRailModule,
      IconTypeSpecificLightDimmerSwitchDinRailModule },
    { ZwavePlusIconType::IconTypeSpecificOnOffPowerSwitchDinRailModule, IconTypeSpecificOnOffPowerSwitchDinRailModule },
    { ZwavePlusIconType::IconTypeGenericThermostat, IconTypeGenericThermostat },
    { ZwavePlusIconType::IconTypeGenericDoorLockKeypad, IconTypeGenericDoorLockKeypad },
    { ZwavePlusIconType::IconTypeGenericEntryControl, IconTypeGenericEntryControl },
    { ZwavePlusIconType::IconTypeSpecificEntryControlRfidTagReaderNoButton,
      IconTypeSpecificEntryControlRfidTagReaderNoButton },
    { ZwavePlusIconType::IconTypeGenericWindowCoveringNoPositionEndpoint,
      IconTypeGenericWindowCoveringNoPositionEndpoint },
    { ZwavePlusIconType::IconTypeGenericWindowCoveringEndpointAware, IconTypeGenericWindowCoveringEndpointAware },
    { ZwavePlusIconType::IconTypeGenericWindowCoveringPositionEndpointAware,
      IconTypeGenericWindowCoveringPositionEndpointAware },
    { ZwavePlusIconType::IconTypeGenericRemoteControlAv, IconTypeGenericRemoteControlAv },
    { ZwavePlusIconType::IconTypeGenericRemoteControlMultiPurpose, IconTypeGenericRemoteControlMultiPurpose },
    { ZwavePlusIconType::IconTypeGenericRemoteControlSimple, IconTypeGenericRemoteControlSimple },
    { ZwavePlusIconType::IconTypeSpecificRemoteControlSimpleKeyfob, IconTypeSpecificRemoteControlSimpleKeyfob },
    { ZwavePlusIconType::IconTypeGenericWallController, IconTypeGenericWallController },
    { ZwavePlusIconType::IconTypeGenericCentralController, IconTypeGenericCentralController },
    { ZwavePlusIconType::IconTypeGenericDisplaySimple, IconTypeGenericDisplaySimple },
    { ZwavePlusIconType::IconTypeGenericGateway, IconTypeGenericGateway },
    { ZwavePlusIconType::IconTypeGenericSetTopBox, IconTypeGenericSetTopBox },
    { ZwavePlusIconType::IconTypeGenericSubSystemController, IconTypeGenericSubSystemController },
    { ZwavePlusIconType::IconTypeGenericTv, IconTypeGenericTv },
    { ZwavePlusIconType::IconTypeGenericRepeater, IconTypeGenericRepeater },
    { ZwavePlusIconType::IconTypeSpecificRepeaterSlave, IconTypeSpecificRepeaterSlave },
    { ZwavePlusIconType::IconTypeSpecificIrRepeater, IconTypeSpecificIrRepeater },
    { ZwavePlusIconType::IconTypeGenericPowerStrip, IconTypeGenericPowerStrip },
    { ZwavePlusIconType::IconTypeGenericSiren, IconTypeGenericSiren },
    { ZwavePlusIconType::IconTypeGenericSoundSwitch, IconTypeGenericSoundSwitch },
    { ZwavePlusIconType::IconTypeSpecificSoundSwitchChime, IconTypeSpecificSoundSwitchChime },
    { ZwavePlusIconType::IconTypeSpecificOnOffWallSwitchDoorBell, IconTypeSpecificOnOffWallSwitchDoorBell },
    { ZwavePlusIconType::IconTypeSpecificSoundSwitchDoorbell, IconTypeSpecificSoundSwitchDoorbell },
    { ZwavePlusIconType::IconTypeSpecificEntryControlKeypad09, IconTypeSpecificEntryControlKeypad09 },
    { ZwavePlusIconType::IconTypeSpecificEntryControlKeypad09OkCancel, IconTypeSpecificEntryControlKeypad09OkCancel },
    { ZwavePlusIconType::IconTypeSpecificEntryControlKeypad09OkCancelHomeStayAway,
      IconTypeSpecificEntryControlKeypad09OkCancelHomeStayAway },
    { ZwavePlusIconType::IconTypeGenericBarrier, IconTypeGenericBarrier },
    { ZwavePlusIconType::IconTypeGenericSensorNotification, IconTypeGenericSensorNotification },
    { ZwavePlusIconType::IconTypeSpecificSensorNotificationSmokeAlarm, IconTypeSpecificSensorNotificationSmokeAlarm },
    { ZwavePlusIconType::IconTypeSpecificSensorNotificationCoAlarm, IconTypeSpecificSensorNotificationCoAlarm },
    { ZwavePlusIconType::IconTypeSpecificSensorNotificationCo2Alarm, IconTypeSpecificSensorNotificationCo2Alarm },
    { ZwavePlusIconType::IconTypeSpecificSensorNotificationHeatAlarm, IconTypeSpecificSensorNotificationHeatAlarm },
    { ZwavePlusIconType::IconTypeSpecificSensorNotificationWaterAlarm, IconTypeSpecificSensorNotificationWaterAlarm },
    { ZwavePlusIconType::IconTypeSpecificSensorNotificationDoorWindow, IconTypeSpecificSensorNotificationDoorWindow },
    { ZwavePlusIconType::IconTypeSpecificSensorNotificationMotionAlarm, IconTypeSpecificSensorNotificationMotionAlarm },
    { ZwavePlusIconType::IconTypeSpecificSensorNotificationPowerManagement,
      IconTypeSpecificSensorNotificationPowerManagement },
    { ZwavePlusIconType::IconTypeSpecificSensorNotificationSystem, IconTypeSpecificSensorNotificationSystem },
    { ZwavePlusIconType::IconTypeSpecificSensorNotificationEmergencyAlarm,
      IconTypeSpecificSensorNotificationEmergencyAlarm },
    { ZwavePlusIconType::IconTypeSpecificSensorNotificationClock, IconTypeSpecificSensorNotificationClock },
    { ZwavePlusIconType::IconTypeSpecificSensorNotificationAppliance, IconTypeSpecificSensorNotificationAppliance },
    { ZwavePlusIconType::IconTypeSpecificSensorNotificationHomeHealth, IconTypeSpecificSensorNotificationHomeHealth },
    { ZwavePlusIconType::IconTypeSpecificSensorNotificationSiren, IconTypeSpecificSensorNotificationSiren },
    { ZwavePlusIconType::IconTypeSpecificSensorNotificationWaterValve, IconTypeSpecificSensorNotificationWaterValve },
    { ZwavePlusIconType::IconTypeSpecificSensorNotificationWeatherAlarm,
      IconTypeSpecificSensorNotificationWeatherAlarm },
    { ZwavePlusIconType::IconTypeSpecificSensorNotificationIrrigation, IconTypeSpecificSensorNotificationIrrigation },
    { ZwavePlusIconType::IconTypeSpecificSensorNotificationGasAlarm, IconTypeSpecificSensorNotificationGasAlarm },
    { ZwavePlusIconType::IconTypeSpecificSensorNotificationPestControl, IconTypeSpecificSensorNotificationPestControl },
    { ZwavePlusIconType::IconTypeSpecificSensorNotificationLightSensor, IconTypeSpecificSensorNotificationLightSensor },
    { ZwavePlusIconType::IconTypeSpecificSensorNotificationMultidevice, IconTypeSpecificSensorNotificationMultidevice },
};

const ZwavePlusIconTypeData* findZwavePlusIconTypeData( ZwavePlusIconType type ) {
    for( const auto& data : zwavePlusIconTypeData ) {
        if( data.type == type ) {
            return &data;
        }
    }

    ZNET_LOGE( "Unsupported ZwavePlusIconTypeData 0x%X\n", uint16_t( type ) );

    return nullptr;
}

}  // namespace

const char* typeToString( ZwavePlusIconType type ) noexcept {
    auto date = findZwavePlusIconTypeData( type );
    return date ? date->name : nullptr;
}

}  // namespace zwave_plus_icon_types_converter
}  // namespace znet
