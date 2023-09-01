#pragma once

#include <znet/znet_visitors.hpp>

namespace znet {

class ZClass;
class ZCcAntiTheftBase;
class ZCc0x2BSceneActivation;
class ZCc0x2CSceneActuatorConf;
class ZCc0x5BCentralScene;
class ZCc0x6FEntryControl;
class ZCc0x20Basic;
class ZCc0x25Switch;
class ZCc0x26SwitchMultiLevel;
class ZCc0x30BinarySensor;
class ZCc0x31SensorMultilevel;
class ZCc0x32Meter;
class ZCc0x33ColorSwitch;
class ZCc0x40ThermostatMode;
class ZCc0x42ThermostatOpState;
class ZCc0x43ThermostatSetpoint;
class ZCc0x44ThermostatFanMode;
class ZCc0x45ThermostatFanState;
class ZCc0x47ThermostatSetback;
class ZCc0x59AssociationGrpInfo;
class ZCc0x62DoorLock;
class ZCc0x66BarrierOperator;
class ZCc0x70Configuration;
class ZCc0x71Notification;
class ZCc0x72ManufacturerSpecific;
class ZCc0x80Battery;
class ZCc0x81Clock;
class ZCc0x84WakeUp;
class ZCc0x85Association;
class ZCc0x8eMultiChannelAssociation;
class ZCc0x9cAlarmSensor;
class ZCc0x9dAlarmSilence;
class ZCc0x63UserCode;
class ZCc0x8ATime;
class ZCc0x8BTimeParameters;
class ZCc0x86Version;
class ZCc0x87Indicator;
class ZCc0x4EScheduleEntryLock;
class ZCc0x60MultiChannel;
class ZCc0x5EZwavePlusInfo;
class ZCc0x79SoundSwitch;
class ZCc0x7AFirmwareUpdate;
class ZCc0x5DAntiTheft;
class ZCc0x7EAntiTheftUnlock;
class ZCc0x98Security;
class ZCc0x9FSecurity2;

using ZClassAcceptor = visitors::BaseAcceptor<ZCc0x2BSceneActivation,
                                              ZCc0x2CSceneActuatorConf,
                                              ZCc0x5BCentralScene,
                                              ZCc0x6FEntryControl,
                                              ZCc0x20Basic,
                                              ZCc0x25Switch,
                                              ZCc0x26SwitchMultiLevel,
                                              ZCc0x30BinarySensor,
                                              ZCc0x31SensorMultilevel,
                                              ZCc0x32Meter,
                                              ZCc0x33ColorSwitch,
                                              ZCc0x40ThermostatMode,
                                              ZCc0x42ThermostatOpState,
                                              ZCc0x43ThermostatSetpoint,
                                              ZCc0x44ThermostatFanMode,
                                              ZCc0x45ThermostatFanState,
                                              ZCc0x47ThermostatSetback,
                                              ZCc0x4EScheduleEntryLock,
                                              ZCc0x59AssociationGrpInfo,
                                              ZCc0x60MultiChannel,
                                              ZCc0x62DoorLock,
                                              ZCc0x66BarrierOperator,
                                              ZCc0x70Configuration,
                                              ZCc0x71Notification,
                                              ZCc0x72ManufacturerSpecific,
                                              ZCc0x80Battery,
                                              ZCc0x81Clock,
                                              ZCc0x84WakeUp,
                                              ZCc0x85Association,
                                              ZCc0x8eMultiChannelAssociation,
                                              ZCc0x9cAlarmSensor,
                                              ZCc0x9dAlarmSilence,
                                              ZCc0x63UserCode,
                                              ZCc0x86Version,
                                              ZCc0x87Indicator,
                                              ZCc0x8ATime,
                                              ZCc0x8BTimeParameters,
                                              ZCc0x5EZwavePlusInfo,
                                              ZCc0x79SoundSwitch,
                                              ZCc0x7AFirmwareUpdate,
                                              ZCc0x5DAntiTheft,
                                              ZCc0x7EAntiTheftUnlock,
                                              ZCc0x98Security,
                                              ZCc0x9FSecurity2>;

using ZClassVisitor = ZClassAcceptor::Visitor;
using ZClassConstVisitor = ZClassAcceptor::ConstVisitor;

}  // namespace znet
