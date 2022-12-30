#include "cored/house_mode_alarms.h"
#include "cored/model/resources/cored_resources.h"

#include "cored/house_modes/house_mode_utils.h"
#include "cored/house_modes/script_events/house_mode_alarmed.h"
#include "cored/house_modes/resources/house_mode_resources.h"

#include "service_interfaces/postponed_task.h"
#include "json_utils/json_utils.h"
#include "database_client/dbqueryparser.h"

#include "config_reader/config_reader.h"
#include "common_utils/cpp14.h"

#include "common_utils/hub_info_helper.h"
#include "service_interfaces/task_loop.h"

#include <chrono>
#include <functional>
#include <utility>

namespace HubPlatform {
namespace CoreD {

namespace {

std::vector< std::string > getSirenItemNames() {
    return { "siren", "switch", "basic" };
}
}  // namespace

using namespace HubPlatform::CoreD::Model::Resources;

HouseModeAlarms::HouseModeAlarms( Services::Service& service, SetItemValue setItemValue )
: m_service( service )
, m_setItemValue( setItemValue ) {}

void HouseModeAlarms::addDeviceToHouseModesAlarmedList( const JsonObject& json, const std::string& id, int delay ) {
    if( delay > 0 ) {
        const auto type = json.find( ItemFields::TypeFieldName );
        auto timestamp = std::chrono::duration_cast< std::chrono::milliseconds >(
                             std::chrono::system_clock::now().time_since_epoch() )
                             .count();

        CoreDHouseModes::addDeviceToHouseModesAlarmedList( m_service.getDBClient(), id, timestamp, delay );
    }
}

void HouseModeAlarms::deleteDeviceFromHouseModesAlarmedList( const std::string& id ) {
    CoreDHouseModes::deleteDeviceFromHouseModesAlarmedList( m_service.getDBClient(), id );
}

void HouseModeAlarms::checkSecurityThreat( const std::string& itemId ) {
    const JsonObject jsonTemp = m_service.getDBClient().findOne(
        Model::Resources::DBNames::TempItemsDB, JSON( Model::Resources::ItemFields::IdFieldName << itemId ), nullptr );
    const JsonObject json = m_service.getDBClient().findOne(
        Model::Resources::DBNames::ItemsDB, JSON( Model::Resources::ItemFields::IdFieldName << itemId ), nullptr );

    const auto itemNameIt = json.find( ItemFields::NameFieldName );
    const auto deviceIdIt = json.find( ItemFields::DeviceIdFieldName );
    const auto valueIt = jsonTemp.find( ItemFields::ValueFieldName );

    const bool allFieldsPresent = itemNameIt != json.end() && deviceIdIt != json.end() && valueIt != jsonTemp.end();
    if( !allFieldsPresent || !valueIt.value().is_boolean() ) {
        return;
    }

    const auto itemName = itemNameIt.value().get< std::string >();
    const std::string deviceId = deviceIdIt.value().get< std::string >();
    const auto value = valueIt.value().get< bool >();

    bool deviceArmed = getDeviceArmedStatusByItem( json );

    if( !deviceArmed ) {
        deleteDeviceFromHouseModesAlarmedList( deviceId );
    }

    if( deviceArmed && itemName == CoreDHouseModes::Resources::Items::SecurityThreat && value ) {
        auto delay = CoreDHouseModes::getAlarmDelay( m_service.getDBClient() );

        auto currentHouseModePtr = CoreDHouseModes::getCurrentHouseMode( m_service.getDBClient() );
        const bool bypassDevice = currentHouseModePtr->m_bypassDeviceIds.count( deviceId ) > 0;
        addDeviceToHouseModesAlarmedList( json, deviceId, delay );

        auto delayBypassThreadLambda = [this, json, deviceId]( Services::PostponedTask& ) -> void {
            bool deviceArmed = getDeviceArmedStatusByItem( json );
            if( deviceArmed ) {
                runSecurityThreatNotificationsWithBroadcast(
                    json, deviceId, [&]() { sendBypassDoneBroadcast( deviceId ); } );
            }

            clearPostponedTasks();
            CoreDHouseModes::clearDeviceFromHouseModesAlarmedList( m_service.getDBClient() );
        };

        const auto modeId = CoreDHouseModes::getCurrentHouseModeId( m_service.getDBClient() );

        auto delayThreadLambda = [this, json, deviceId, modeId, delay]( Services::PostponedTask& ) -> void {
            std::lock_guard< std::mutex > lock( m_postponedMutex );
            bool deviceArmed = getDeviceArmedStatusByItem( json );
            if( deviceArmed ) {
                runSecurityThreatNotificationsWithBroadcast( json, deviceId, [&]() {
                    sendEntryDelayDoneBroadcast( deviceId );
                    sendMainDoneBroadcast( deviceId );
                } );
                clearPostponedTasks();
                CoreDHouseModes::clearDeviceFromHouseModesAlarmedList( m_service.getDBClient() );
            }
        };

        auto updateTasks = [this]( Services::PostponedTaskPtr& task, const std::string deviceId ) {
            auto timestamp = std::chrono::duration_cast< std::chrono::milliseconds >(
                                 std::chrono::system_clock::now().time_since_epoch() )
                                 .count();
            auto newTask = std::make_pair( task, timestamp );
            auto tasks = m_postponedTasks[ deviceId ];
            tasks.push_back( newTask );
            m_postponedTasks[ deviceId ] = tasks;
        };

        if( bypassDevice ) {
            auto task = m_service.createPostponedTask( delayBypassThreadLambda );
            if( m_postponedTasks.empty() ) {
                task->start( std::chrono::seconds( delay ), false );
            }

            updateTasks( task, deviceId );
        }
        else {
            if( delay > 0 ) {
                std::lock_guard< std::mutex > lock( m_postponedMutex );
                auto task = m_service.createPostponedTask( delayThreadLambda );
                if( m_postponedTasks.empty() ) {
                    sendEntryDelayBeginBroadcast( deviceId );
                    task->start( std::chrono::seconds( delay ), false );
                }

                updateTasks( task, deviceId );
            }
            else {
                runSecurityThreatNotificationsWithBroadcast(
                    json, deviceId, [this, deviceId]() { sendMainDoneBroadcast( deviceId ); } );
            }
        }
    }
}

bool HouseModeAlarms::getDeviceArmedStatusByItem( const JsonObject& item ) {
    const auto deviceId = json_get_str( item, ItemFields::DeviceIdFieldName );
    const JsonObject device =
        m_service.getDBClient().findOne( DBNames::DevicesDB, JSON( db::ID_FIELD_NAME << deviceId ), nullptr );
    return json_get_bool( device, DeviceFields::ArmedFieldName, false );
}

void HouseModeAlarms::sendEntryDelayBeginBroadcast( const std::string& deviceId ) {
    const auto modeId = CoreDHouseModes::getCurrentHouseModeId( m_service.getDBClient() );
    auto delay = CoreDHouseModes::getAlarmDelay( m_service.getDBClient() );
    CoreDHouseModes::sendHouseModeAlarmedBroadcast(
        m_service, modeId, RPCParams::Begin, RPCParams::EntryDelay, deviceId, delay );
}

void HouseModeAlarms::sendEntryDelayDoneBroadcast( const std::string& deviceId ) {
    const auto modeId = CoreDHouseModes::getCurrentHouseModeId( m_service.getDBClient() );
    auto delay = CoreDHouseModes::getAlarmDelay( m_service.getDBClient() );
    CoreDHouseModes::sendHouseModeAlarmedBroadcast(
        m_service, modeId, RPCParams::Done, RPCParams::EntryDelay, deviceId, delay );
}

void HouseModeAlarms::sendEntryDelayCanceledBroadcast() {
    const auto modeId = CoreDHouseModes::getCurrentHouseModeId( m_service.getDBClient() );
    auto delay = CoreDHouseModes::getAlarmDelay( m_service.getDBClient() );
    CoreDHouseModes::sendHouseModeAlarmedBroadcast(
        m_service, modeId, RPCParams::Canceled, RPCParams::EntryDelay, "", delay );
}

void HouseModeAlarms::sendMainDoneBroadcast( const std::string& deviceId ) {
    const auto modeId = CoreDHouseModes::getCurrentHouseModeId( m_service.getDBClient() );
    auto delay = CoreDHouseModes::getAlarmDelay( m_service.getDBClient() );
    CoreDHouseModes::sendHouseModeAlarmedBroadcast(
        m_service, modeId, RPCParams::Done, RPCParams::Main, deviceId, delay );
}

void HouseModeAlarms::sendBypassDoneBroadcast( const std::string& deviceId ) {
    const auto modeId = CoreDHouseModes::getCurrentHouseModeId( m_service.getDBClient() );
    auto delay = CoreDHouseModes::getAlarmDelay( m_service.getDBClient() );
    CoreDHouseModes::sendHouseModeAlarmedBroadcast(
        m_service, modeId, RPCParams::Done, RPCParams::Bypass, deviceId, delay );
}

void HouseModeAlarms::clearPostponedTasks( const std::string& exceptedId ) {
    for( PostponedTasks::iterator it = m_postponedTasks.begin(); it != m_postponedTasks.end(); ) {
        const auto& deviceId = it->first;

        if( deviceId != exceptedId ) {
            auto& tasks = it->second;

            for( auto taskItem : tasks ) {
                auto task = taskItem.first;
                task->stop();
                task.reset();
            }

            it = m_postponedTasks.erase( it );
        }
        else {
            it++;
        }
    }
}

void HouseModeAlarms::runSecurityThreatNotificationsWithBroadcast( const JsonObject& json,
                                                                   const DeviceID& deviceId,
                                                                   std::function< void() > emitBroadcast ) {
    static const std::vector< std::string > sirenItemNames = getSirenItemNames();
    static const std::vector< std::string > cameraItemNames = { CoreDHouseModes::Resources::Items::MakeRecording };

    auto currentHouseModePtr = CoreDHouseModes::getCurrentHouseMode( m_service.getDBClient() );
    const bool bypassDevice = currentHouseModePtr->m_bypassDeviceIds.count( deviceId ) > 0;

    m_service.emit( CoreDHouseModes::ScriptEvents::HouseModeAlarmed( currentHouseModePtr->m_id, deviceId ) );
    if( emitBroadcast != nullptr ) {
        emitBroadcast();
    }

    if( !bypassDevice && currentHouseModePtr != nullptr ) {
        notifyAllAlarms( sirenItemNames, *currentHouseModePtr );
    }

    if( currentHouseModePtr != nullptr ) {
        notifyAllCameras( cameraItemNames, *currentHouseModePtr );
    }

    deleteDeviceFromHouseModesAlarmedList( deviceId );
}

void HouseModeAlarms::notifyAllAlarms( const std::vector< std::string >& alarmItemNames,
                                       CoreDHouseModes::HouseModeEntry& currentHouseMode ) {
    const auto& alarmOffDevices = currentHouseMode.m_alarmsOffDeviceIds;
    auto alarmDeviceIdsVector = CoreDHouseModes::getAlarmDeviceIDsAffectedByHouseModes( m_service.getDBClient() );
    auto alarmDeviceIds = std::set< std::string >( alarmDeviceIdsVector.begin(), alarmDeviceIdsVector.end() );
    std::set< std::string > deviceIds;
    std::set_difference( alarmDeviceIds.begin(),
                         alarmDeviceIds.end(),
                         alarmOffDevices.begin(),
                         alarmOffDevices.end(),
                         std::inserter( deviceIds, deviceIds.begin() ) );
    for( const auto& alarmDeviceId : deviceIds ) {
        auto items = m_service.getDBClient().findAll(
            DBNames::ItemsDB, JSON( ItemFields::DeviceIdFieldName << alarmDeviceId ), nullptr );
        for( auto& sirenItemName : alarmItemNames ) {
            auto it = std::find_if( items.begin(), items.end(), [sirenItemName]( const JsonObject& item ) {
                return json_get_str( item, ItemFields::NameFieldName ) == sirenItemName;
            } );
            if( it != items.end() ) {
                const auto itemId = json_get_str( *it, ItemFields::IdFieldName );
                const JsonObject setItemValueRequest{ { db::ID_FIELD_NAME, itemId },
                                                      { ItemFields::ValueFieldName, true },
                                                      { RPCParams::SourceField,
                                                        { { ItemSource::TypeField, ItemSource::HouseMode } } } };
                m_setItemValue( setItemValueRequest );
                break;
            }
        }
    }
}

void HouseModeAlarms::notifyAllCameras( const std::vector< std::string >& cameraItemNames,
                                        CoreDHouseModes::HouseModeEntry& currentHouseMode ) {
    using namespace HubPlatform::CoreDHouseModes::Resources;
    const auto& camerasOffDevices = currentHouseMode.m_camerasOffDeviceIds;
    std::set< std::string > cameraDeviceIds;
    std::map< std::string, std::string > cameraDeviceItems;

    auto buildCollections = [&cameraDeviceIds, &cameraDeviceItems]( const Json& fields ) {
        const auto& deviceId = json_get_str( fields, db::ID_FIELD_NAME );
        const auto& itemId = json_get_str( fields, Items::MakeRecording );
        cameraDeviceIds.insert( deviceId );
        cameraDeviceItems.emplace( deviceId, itemId );
    };

    if( 0 == CoreDHouseModes::getCamerasDeviceItemPairs( m_service.getDBClient(), JSON_EMPTY_OBJ, buildCollections ) )
        return;

    std::set< std::string > deviceIdsToNotify;
    std::set_difference( cameraDeviceIds.begin(),
                         cameraDeviceIds.end(),
                         camerasOffDevices.begin(),
                         camerasOffDevices.end(),
                         std::inserter( deviceIdsToNotify, deviceIdsToNotify.begin() ) );
    for( const auto& cameraDeviceId : deviceIdsToNotify ) {
        auto foundIt = cameraDeviceItems.find( cameraDeviceId );
        if( foundIt != cameraDeviceItems.end() ) {
            const auto& itemId = foundIt->second;

            const int DefaultPlaybackTime = 0;
            const JsonObject setItemValueRequest{ { db::ID_FIELD_NAME, itemId },
                                                  { ItemFields::ValueFieldName, DefaultPlaybackTime },
                                                  { RPCParams::SourceField,
                                                    { { ItemSource::TypeField, ItemSource::HouseMode } } } };
            m_setItemValue( setItemValueRequest );
            LOGGER_TRACE( cc::trace( "handleSetItemValue called for itemId=" ) + itemId );

            break;
        }
    }
}

void HouseModeAlarms::turnOffAlarms( const std::vector< std::string >& alarmItemNames ) {
    CoreDHouseModes::DeviceIDs alarmIds =
        CoreDHouseModes::getAlarmDeviceIDsAffectedByHouseModes( m_service.getDBClient() );

    Json fields = JSON( ItemFields::IdFieldName << 1 << ItemFields::NameFieldName << 1 );
    for( const auto& alarmDeviceId : alarmIds ) {
        auto items = m_service.getDBClient().findAll(
            DBNames::ItemsDB, JSON( ItemFields::DeviceIdFieldName << alarmDeviceId ), &fields );
        const bool alarmNewValue = false;

        for( auto& sirenItemName : alarmItemNames ) {
            auto it = std::find_if( items.begin(), items.end(), [sirenItemName]( const JsonObject& item ) {
                return json_get_str( item, ItemFields::NameFieldName ) == sirenItemName;
            } );
            if( it != items.end() ) {
                const auto itemId = json_get_str( *it, ItemFields::IdFieldName );
                const JsonObject setItemValueRequest{ { db::ID_FIELD_NAME, itemId },
                                                      { ItemFields::ValueFieldName, alarmNewValue },
                                                      { RPCParams::SourceField,
                                                        { { ItemSource::TypeField, ItemSource::HouseMode } } } };
                m_setItemValue( setItemValueRequest );
                break;
            }
        }
    }
}

void HouseModeAlarms::houseModeSwitchFromArmedToDisarmedMode( const JsonObject& message ) {
    int switchToDelay = json_get_int( message, "switchToDelay", -1 );
    if( switchToDelay != 0 )
        return;
    std::string status = json_get_str( message, "status" );
    if( status != "done" )
        return;
    std::string modeFrom = json_get_str( message, "from" );
    std::string modeTo = json_get_str( message, "to" );

    auto modeFromEntry = CoreDHouseModes::getHouseMode( m_service.getDBClient(), modeFrom );
    if( modeFromEntry && modeFromEntry->m_armed ) {
        auto modeToEntry = CoreDHouseModes::getHouseMode( m_service.getDBClient(), modeTo );
        if( modeToEntry && !modeToEntry->m_armed ) {
            turnOffAlarms( getSirenItemNames() );
            if( !m_postponedTasks.empty() ) {
                clearAllPendingSecurityEventsAndCountdown();
            }
        }
    }
}

void HouseModeAlarms::clearAllPendingSecurityEventsAndCountdown() {
    clearPostponedTasks();
    sendEntryDelayCanceledBroadcast();
    CoreDHouseModes::clearDeviceFromHouseModesAlarmedList( m_service.getDBClient() );
}

void HouseModeAlarms::cancelEntryDelay() {
    clearAllPendingSecurityEventsAndCountdown();
}

}  // namespace CoreD
}  // namespace HubPlatform
