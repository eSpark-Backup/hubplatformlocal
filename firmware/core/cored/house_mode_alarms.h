#pragma once

#include "service_interfaces/service.h"
#include "cored/house_mode_proc.h"
#include "service_interfaces/script_event.h"
#include "json_utils/json_utils.h"

#include <memory>
#include <string>

namespace HubPlatform {
namespace CoreD {

class HouseModeAlarms {
    using SetItemValue = std::function< void( const JsonObject& ) >;
    using Emit = std::function< void( const Services::ScriptEvent& ) >;
    using PostponedTask = std::function< Services::PostponedTaskPtr( Services::PostponedTaskFunction ) >;
    using Timestamp = unsigned long;

public:
    HouseModeAlarms( Services::Service& service, SetItemValue setItemValue );
    void houseModeSwitchFromArmedToDisarmedMode( const JsonObject& message );
    void checkSecurityThreat( const std::string& itemId );
    void cancelEntryDelay();
    void skipEntryDelay( const JsonObject& json );

private:
    using TaskItem = std::pair< Services::PostponedTaskPtr, Timestamp >;
    using PostponedTasks = std::map< std::string, std::vector< TaskItem > >;
    using DeviceID = std::string;

    bool getDeviceArmedStatusByItem( const JsonObject& item );
    void addDeviceToHouseModesAlarmedList( const JsonObject& json, const std::string& deviceId, int delay );
    void deleteDeviceFromHouseModesAlarmedList( const std::string& id );
    void clearPostponedTasks( const std::string& exceptedId = "" );
    void sendEntryDelayBeginBroadcast( const std::string& deviceId );
    void sendEntryDelayDoneBroadcast( const std::string& deviceId );
    void sendEntryDelayCanceledBroadcast();
    void sendMainDoneBroadcast( const std::string& deviceId );
    void sendBypassDoneBroadcast( const std::string& deviceId );
    void sendMainBeginBroadcast( const std::string& deviceId );

    void turnOffAlarms( const std::vector< std::string >& alarmItemNames );
    void notifyAllCameras( const std::vector< std::string >& cameraItemNames,
                           CoreDHouseModes::HouseModeEntry& currentHouseMode );
    void notifyAllAlarms( const std::vector< std::string >& alarmItemNames,
                          CoreDHouseModes::HouseModeEntry& currentHouseMode );

    void runSecurityThreatNotificationsWithBroadcast( const JsonObject& json,
                                                      const DeviceID& deviceId,
                                                      std::function< void() > emitBroadcast );
    void clearAllPendingSecurityEventsAndCountdown();

private:
    std::unique_ptr< ConfigReader > m_configReader;

    std::mutex m_postponedMutex;
    PostponedTasks m_postponedTasks;

    Services::Service& m_service;
    SetItemValue m_setItemValue;
};

}  // namespace CoreD
}  // namespace HubPlatform
