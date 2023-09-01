#pragma once

#include <map>
#include <string>
#include "database_client/db_tables_names.h"

namespace HubPlatform {
namespace CoreDHouseModes {
namespace Resources {

namespace DBNames {

extern const char* const HouseModesDB;
extern const char* const HouseModesProtectButtonsDB;
extern const char* const HouseModesCamerasDB;
extern const char* const HouseModesProtectDevicesDB;
extern const char* const HouseModesSecurityThreatsDB;

}  // namespace DBNames

namespace HouseModeFields {

extern const char* const IdFieldName;
extern const char* const ModeNameFieldName;
extern const char* const DescriptionFieldName;
extern const char* const SwitchToDelayFieldName;
extern const char* const AlarmDelayFieldName;
extern const char* const DisarmedDefaultFieldName;
extern const char* const DisarmedDevicesFieldName;
extern const char* const ArmedCategoriesFieldName;
extern const char* const CategoryFieldName;
extern const char* const SubcategoriesFieldName;
extern const char* const NotificationsFieldName;
extern const char* const NotificationsDefaultFieldName;
extern const char* const AlarmsOffDevicesFieldName;
extern const char* const AlarmCategoriesFieldName;
extern const char* const CamerasOffDevicesFieldName;
extern const char* const BypassDevicesFieldName;
extern const char* const IgnoreSwitchDelay;
extern const char* const ProtectFieldName;
extern const char* const ArmedFieldName;

}  // namespace HouseModeFields

namespace ScriptEvents {

namespace Name {

extern const char* const HouseModeSwitched;
extern const char* const HouseModeAlarmed;

}  // namespace Name

}  // namespace ScriptEvents

namespace ProtectService {
extern const char* const Police;
extern const char* const Medical;
extern const char* const Fire;
}  // namespace ProtectService

namespace BroadcastFields {
extern const char* const ModeIdFieldName;
extern const char* const AlarmsOffDeviceFieldName;
extern const char* const DisarmedDeviceFieldName;
extern const char* const CamerasOffDeviceFieldName;
extern const char* const DeviceIdsFieldName;
extern const char* const DeviceIdFieldName;
extern const char* const DeviceTimestampFieldName;
extern const char* const DevicePhaseFieldName;
extern const char* const DeviceStatusFieldName;
extern const char* const DevicePendingDelay;
}  // namespace BroadcastFields

namespace Items {
extern const char* const MakeRecording;
extern const char* const SecurityThreat;
}  // namespace Items

}  // namespace Resources
}  // namespace CoreDHouseModes
}  // namespace HubPlatform
