#pragma once

namespace znet {

enum class GenericType {
    GenericTypeSwitchMultilevel = 0x11,
    GenericTypeSwitchBinary = 0x10,
    GenericTypeSwitchRemote = 0x012,
    GenericTypeSwitchToggle = 0x13,
    GenericTypeThermostat = 0x08,
    GenericTypeEntryControl = 0x40,
    GenericTypeWindowCovering = 0x09,
    GenericTypeGenericController = 0x01,
    GenericTypeWallController = 0x18,
    GenericTypeStaticController = 0x02,
    GenericTypeRepeaterSlave = 0x0F,
    GenericTypeNetworkExtender = 0x05,
    GenericTypeAvControlPoint = 0x03,
    GenericTypeAppliance = 0x06,
    GenericTypeSensorNotification = 0x07,
    GenericTypeSecurityPanel = 0x17,
    GenericTypeSensorBinary = 0x20,
    GenericTypeSensorMultilevel = 0x21
};

}  // namespace znet
