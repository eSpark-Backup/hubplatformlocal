#include <znet/converters/specific_types_converter.hpp>

#include <znet/enums/generic_type.hpp>
#include <znet/converters/resources.hpp>
#include <znet/znet_utils.hpp>

extern "C" {
#include <znet_log.h>
}

namespace znet {
namespace specific_types_converter {

using namespace resources::specific_types;

const char* typeToString( GenericType genericType, uint8_t specificType ) noexcept {
    switch( genericType ) {
    case GenericType::GenericTypeSwitchMultilevel:
        switch( specificType ) {
        case 0x00:
            return SpecificTypeNotUsed;
        case 0x01:
            return SpecificTypePowerSwitchMultilevel;
        case 0x02:
            return SpecificTypeColorTunableMultilevel;
        case 0x03:
            return SpecificTypeMotorMultiposition;
        case 0x04:
            return SpecificTypeSceneSwitchMultilevel;
        case 0x05:
            return SpecificTypeClassAMotorControl;
        case 0x06:
            return SpecificTypeClassBMotorControl;
        case 0x07:
            return SpecificTypeClassCMotorControl;
        case 0x08:
            return SpecificTypeFanSwitch;
        }
        break;

    case GenericType::GenericTypeSwitchBinary:
        switch( specificType ) {
        case 0x00:
            return SpecificTypeNotUsed;
        case 0x01:
            return SpecificTypePowerSwitchBinary;
        case 0x02:
            return SpecificTypeColorTunableBinary;
        case 0x03:
            return SpecificTypeSceneSwitchBinary;
        case 0x04:
            return SpecificTypePowerStrip;
        case 0x05:
            return SpecificTypeSiren;
        case 0x06:
            return SpecificTypeValveOpenClose;
        case 0x07:
            return SpecificTypeIrrigationController;
        }
        break;

    case GenericType::GenericTypeSwitchRemote:
        switch( specificType ) {
        case 0x00:
            return SpecificTypeNotUsed;
        case 0x01:
            return SpecificTypeSwitchRemoteBinary;
        case 0x02:
            return SpecificTypeSwitchRemoteMultilevel;
        case 0x03:
            return SpecificTypeSwitchRemoteToggleBinary;
        case 0x04:
            return SpecificTypeSwitchRemoteToggleMultilevel;
        }
        break;

    case GenericType::GenericTypeSwitchToggle:
        switch( specificType ) {
        case 0x00:
            return SpecificTypeNotUsed;
        case 0x01:
            return SpecificTypeSwitchToggleBinary;
        case 0x02:
            return SpecificTypeSwitchToggleMultilevel;
        }
        break;

    case GenericType::GenericTypeThermostat:
        switch( specificType ) {
        case 0x00:
            return SpecificTypeNotUsed;
        case 0x01:
            return SpecificTypeThermostatHeating;
        case 0x02:
            return SpecificTypeThermostatGeneral;
        case 0x06:
            return SpecificTypeThermostatGeneralV2;
        }
        break;

    case GenericType::GenericTypeEntryControl:
        switch( specificType ) {
        case 0x00:
            return SpecificTypeNotUsed;
        case 0x01:
            return SpecificTypeDoorLock;
        case 0x02:
            return SpecificTypeAdvancedDoorLock;
        case 0x03:
            return SpecificTypeSecureKeypadDoorLock;
        case 0x04:
            return SpecificTypeSecureKeypadDoorLockDeadbolt;
        case 0x05:
            return SpecificTypeSecureDoor;
        case 0x06:
            return SpecificTypeSecureGate;
        case 0x07:
            return SpecificTypeSecureBarrierAddon;
        case 0x08:
            return SpecificTypeSecureBarrierOpenOnly;
        case 0x09:
            return SpecificTypeSecureBarrierCloseOnly;
        case 0x0A:
            return SpecificTypeSecureLockbox;
        case 0x0B:
            return SpecificTypeSecureKeypad;
        }
        break;

    case GenericType::GenericTypeWindowCovering:
        switch( specificType ) {
        case 0x00:
            return SpecificTypeNotUsed;
        case 0x01:
            return SpecificTypeSimpleWindowCovering;
        }
        break;

    case GenericType::GenericTypeGenericController:
        switch( specificType ) {
        case 0x00:
            return SpecificTypeNotUsed;
        case 0x01:
            return SpecificTypePortableRemoteController;
        case 0x02:
            return SpecificTypePortableSceneController;
        case 0x03:
            return SpecificTypePortableInstallerTool;
        case 0x04:
            return SpecificTypeRemoteControlAv;
        case 0x06:
            return SpecificTypeRemoteControlSimple;
        }
        break;

    case GenericType::GenericTypeWallController:
        switch( specificType ) {
        case 0x00:
            return SpecificTypeNotUsed;
        case 0x01:
            return SpecificTypeBasicWallController;
        }
        break;

    case GenericType::GenericTypeStaticController:
        switch( specificType ) {
        case 0x00:
            return SpecificTypeNotUsed;
        case 0x01:
            return SpecificTypePcController;
        case 0x02:
            return SpecificTypeSceneController;
        case 0x03:
            return SpecificTypeStaticInstallerTool;
        case 0x04:
            return SpecificTypeSetTopBox;
        case 0x05:
            return SpecificTypeSubSystemController;
        case 0x06:
            return SpecificTypeTv;
        case 0x07:
            return SpecificTypeGateway;
        }
        break;

    case GenericType::GenericTypeRepeaterSlave:
        switch( specificType ) {
        case 0x00:
            return SpecificTypeNotUsed;
        case 0x01:
            return SpecificTypeRepeaterSlave;
        case 0x02:
            return SpecificTypeVirtualNode;
        }
        break;

    case GenericType::GenericTypeNetworkExtender:
        switch( specificType ) {
        case 0x00:
            return SpecificTypeNotUsed;
        case 0x01:
            return SpecificTypeSecureExtender;
        }
        break;

    case GenericType::GenericTypeAvControlPoint:
        switch( specificType ) {
        case 0x00:
            return SpecificTypeNotUsed;
        case 0x01:
            return SpecificTypeSoundSwitch;
        case 0x04:
            return SpecificTypeSatelliteReceiver;
        case 0x11:
            return SpecificTypeSatelliteReceiverV2;
        case 0x12:
            return SpecificTypeDoorbell;
        }
        break;

    case GenericType::GenericTypeAppliance:
        switch( specificType ) {
        case 0x00:
            return SpecificTypeNotUsed;
        case 0x01:
            return SpecificTypeGeneralAppliance;
        case 0x02:
            return SpecificTypeKitchenAppliance;
        case 0x03:
            return SpecificTypeLaundryAppliance;
        }
        break;

    case GenericType::GenericTypeSensorNotification:
        switch( specificType ) {
        case 0x00:
            return SpecificTypeNotUsed;
        case 0x01:
            return SpecificTypeNotificationSensor;
        }
        break;

    case GenericType::GenericTypeSecurityPanel:
        switch( specificType ) {
        case 0x00:
            return SpecificTypeNotUsed;
        case 0x01:
            return SpecificTypeZonedSecurityPanel;
        }
        break;

    case GenericType::GenericTypeSensorBinary:
        switch( specificType ) {
        case 0x01:
            return SpecificTypeRoutingSensorBinary;
        }
        break;

    case GenericType::GenericTypeSensorMultilevel:
        switch( specificType ) {
        case 0x00:
            return SpecificTypeNotUsed;
        case 0x01:
            return SpecificTypeRoutingSensorMultilevel;
        }
        break;
    }

    ZNET_LOGE( "Unsupported SpecificTypeData: generic: 0x%x, specific: 0x%x \n", genericType, specificType );

    return Unknown;
}

}  // namespace specific_types_converter
}  // namespace znet
