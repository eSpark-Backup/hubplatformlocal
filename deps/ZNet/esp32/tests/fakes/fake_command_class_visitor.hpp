#pragma once

#include <znet/znet_command_class_visitor.hpp>

namespace znet {
namespace ut {
struct FakeZClassVistor : ZClassVisitor {
    void operator()( ZCc0x2BSceneActivation& cc ) override {}
    void operator()( ZCc0x2CSceneActuatorConf& cc ) override {}
    void operator()( ZCc0x5BCentralScene& cc ) override {}
    void operator()( ZCc0x6FEntryControl& cc ) override {}
    void operator()( ZCc0x20Basic& cc ) override {}
    void operator()( ZCc0x25Switch& cc ) override {}
    void operator()( ZCc0x26SwitchMultiLevel& cc ) override {}
    void operator()( ZCc0x63UserCode& cc ) override {}
    void operator()( ZCc0x30BinarySensor& cc ) override {}
    void operator()( ZCc0x31SensorMultilevel& cc ) override {}
    void operator()( ZCc0x32Meter& cc ) override {}
    void operator()( ZCc0x33ColorSwitch& cc ) override {}
    void operator()( ZCc0x40ThermostatMode& cc ) override {}
    void operator()( ZCc0x42ThermostatOpState& cc ) override {}
    void operator()( ZCc0x43ThermostatSetpoint& cc ) override {}
    void operator()( ZCc0x44ThermostatFanMode& cc ) override {}
    void operator()( ZCc0x45ThermostatFanState& cc ) override {}
    void operator()( ZCc0x47ThermostatSetback& cc ) override {}
    void operator()( ZCc0x4EScheduleEntryLock& cc ) override {}
    void operator()( ZCc0x59AssociationGrpInfo& cc ) override {}
    void operator()( ZCc0x60MultiChannel& cc ) override {}
    void operator()( ZCc0x62DoorLock& cc ) override {}
    void operator()( ZCc0x66BarrierOperator& cc ) override {}
    void operator()( ZCc0x70Configuration& cc ) override {}
    void operator()( ZCc0x71Notification& cc ) override {}
    void operator()( ZCc0x72ManufacturerSpecific& cc ) override {}
    void operator()( ZCc0x80Battery& cc ) override {}
    void operator()( ZCc0x81Clock& cc ) override {}
    void operator()( ZCc0x84WakeUp& cc ) override {}
    void operator()( ZCc0x85Association& cc ) override {}
    void operator()( ZCc0x8eMultiChannelAssociation& cc ) override {}
    void operator()( ZCc0x9cAlarmSensor& cc ) override {}
    void operator()( ZCc0x9dAlarmSilence& cc ) override {}
    void operator()( ZCc0x86Version& cc ) override {}
    void operator()( ZCc0x87Indicator& cc ) override {}
    void operator()( ZCc0x8ATime& cc ) override {}
    void operator()( ZCc0x8BTimeParameters& cc ) override {}
    void operator()( ZCc0x5EZwavePlusInfo& cc ) override {}
    void operator()( ZCc0x79SoundSwitch& cc ) override {}
    void operator()( ZCc0x7AFirmwareUpdate& cc ) override {}
    void operator()( ZCc0x5DAntiTheft& cc ) override {}
    void operator()( ZCc0x7EAntiTheftUnlock& cc ) override {}
    void operator()( ZCc0x98Security& cc ) override {}
    void operator()( ZCc0x9FSecurity2& cc ) override {}
};
}  // namespace ut
}  // namespace znet
