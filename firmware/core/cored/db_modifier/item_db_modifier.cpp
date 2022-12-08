#include "cored/db_modifier/item_db_modifier.h"
#include "cored/features_manager.h"

#include "cored/house_modes/resources/house_mode_resources.h"
#include "cored/house_modes/house_mode_utils.h"
#include "cored/house_modes/resources/house_mode_resources.h"

#include "database_client/dbqueryparser.h"
#include "database_client/idbclient.h"

#include "cored/model/resources/cored_resources.h"
#include "cored/model/configuration/configuration_factory.h"
#include "cored/model/visitors/ui_value_writer_visitor.h"
#include "cored/model/visitors/ui_value_formatted_getter_visitior.h"
#include "cored/model/values/value_factory.h"
#include "cored/model/values/value.h"
#include "cored/model/values/value_type.h"
#include "cored/model/visitors/item_value_float_visitor.h"
#include "cored/db_modifier/power_broadcast_modifier.h"

#include "service_interfaces/service.h"
#include "json_utils/json_custom_getters.h"

#include <stdexcept>
#include <cassert>
#include <type_traits>

namespace HubPlatform {
namespace CoreD {

namespace {
std::string getElementsMaxNumberKeyName( const Model::ValueType& valueType,
                                         Model::Item::ContainerLevel containerLevel ) {
    if( containerLevel == 0 )
        return "elementsMaxNumber";

    if( containerLevel == 1 && valueType.isArray() )
        return "elementsMaxNumberPerArray";

    assert( !"Please, add custom elementsMaxNumber for current container" );
    return "elementsMaxNumber" + std::to_string( containerLevel + 1 );
}
}  // namespace

class DbModifierError : public std::runtime_error {
public:
    explicit DbModifierError( const std::string& errDescription )
    : runtime_error( "DbModifier: " + errDescription ) {}
};

ItemDbModifier::ItemDbModifier( Services::Service& service,
                                const ProvisionDataUtils::ProvisionData& provisionData,
                                Broadcaster broadcaster,
                                const IFeaturesManager& featureManager )
: DbModifier( service.getDBClient(),
              std::move( broadcaster ),
              Model::Resources::DBNames::ItemsDB,
              { { HUB_ITEM_ADDED, {} }, { HUB_ITEM_UPDATED, {} }, { HUB_ITEM_REMOVED, {} } },
              { Model::Resources::ItemFields::ValueFieldName } )
, m_service( service )
, m_provisionData( provisionData )
, m_featureManager( featureManager ) {
    m_modifyBroadcast.emplace_back( std::make_unique< PowerBroadcastModifier >( service.getDBClient() ) );
    m_modifyBroadcast.shrink_to_fit();
}

Json& ItemDbModifier::prepareUpdateBroadcastMessage( Json& broadcastData, const Json& oldFieldsValues ) {
    using namespace HubPlatform::CoreD::Model::Resources;
    const auto ItemId = json_get_str( broadcastData, ItemFields::IdFieldName );
    const auto item = m_db.findOne( DBNames::ItemsDB, JSON( ItemFields::IdFieldName << ItemId ), nullptr );

    const auto deviceId = json_get_str( item, ItemFields::DeviceIdFieldName );

    broadcastData[ System::SyncNotification ] = json_contains( broadcastData, ItemFields::NameFieldName );
    const auto name = json_get_str( item, ItemFields::NameFieldName );

    std::string deviceName;
    std::string parentDeviceId;

    std::string deviceCategory;
    std::string deviceSubcategory;

    const auto device = m_db.findOne( DBNames::DevicesDB, JSON( DeviceFields::IdFieldName << deviceId ), nullptr );
    if( device.empty() ) {
        Log::warn( std::string( "Item '" ) + ItemId + "' doesn't have related device(" + deviceId + ")" );
    }
    else {
        deviceName = json_get_str( device, DeviceFields::NameFieldName );
        if( device.count( DeviceFields::ArmedFieldName ) >= 1 ) {
            broadcastData[ "deviceArmed" ] = json_get_bool( device, DeviceFields::ArmedFieldName );
        }

        parentDeviceId = json_get_str( device, DeviceFields::ParentDeviceIdFieldName );

        deviceCategory = json_get_str( device, DeviceFields::CategoryFieldName );
        deviceSubcategory = json_get_str( device, DeviceFields::SubcategoryFieldName );
        bool serviceNotificationFlag;
        if( json_get( device, DeviceFields::ServiceNotificationFieldName, serviceNotificationFlag ) ) {
            broadcastData[ DeviceFields::ServiceNotificationFieldName ] = serviceNotificationFlag;
        }
    }

    std::string roomName;
    const auto roomId = json_get_str( device, DeviceFields::RoomIdFieldName );
    const auto roomObj = m_db.findOne( DBNames::RoomsDB, JSON( RoomFields::IdFieldName << roomId ), nullptr );
    if( !roomObj.empty() ) {
        roomName = json_get_str( roomObj, RoomFields::NameFieldName );
    }

    const auto currentHouseMode = CoreDHouseModes::getCurrentHouseMode( m_db );
    CoreDHouseModes::HouseModeEntry::notificationsToJson( broadcastData[ "notifications" ],
                                                          currentHouseMode->m_notificationUserIds );
    broadcastData[ DictionaryEditRequestFields::DeviceIdFieldName ] = deviceId;
    broadcastData[ "deviceName" ] = deviceName;
    broadcastData[ "deviceCategory" ] = deviceCategory;
    broadcastData[ "deviceSubcategory" ] = deviceSubcategory;
    broadcastData[ "roomName" ] = roomName;
    broadcastData[ DictionaryEditRequestFields::NameFieldName ] = name;

    if( !parentDeviceId.empty() ) {
        broadcastData[ "parentDeviceId" ] = parentDeviceId;
    }

    if( item.count( ItemFields::UserCodeRestrictionFieldName ) ) {
        broadcastData[ "userCodeRestriction" ] = json_get_str( item, ItemFields::UserCodeRestrictionFieldName );
    }

    if( item.count( ItemFields::UserCodeModesFieldName ) ) {
        broadcastData[ "userCodeModes" ] = item[ ItemFields::UserCodeModesFieldName ];
    }

    if( item.count( ItemFields::StringRestrictionFieldName ) ) {
        broadcastData[ "stringRestriction" ] = json_get_str( item, ItemFields::StringRestrictionFieldName );
    }

    // Check all notifiers and fill the broadcast with custom fields if needed.
    auto prepareUpdateBroadcast =
        [&name, &deviceCategory, &deviceSubcategory, &broadcastData]( const BroadcastModifierUPtr& notifyUser ) {
            return notifyUser->prepareUpdateBroadcast( name, deviceCategory, deviceSubcategory, broadcastData );
        };
    bool isPrepared = std::any_of( begin( m_modifyBroadcast ), end( m_modifyBroadcast ), prepareUpdateBroadcast );
    if( !isPrepared ) {
        broadcastData[ "userNotification" ] =
            shouldUserBeNotified( name, deviceCategory, deviceSubcategory, broadcastData, oldFieldsValues );
    }

    if( sendEzloProtectNotification( *currentHouseMode, deviceId ) ) {
        prepareProtectServiceField( broadcastData, json_get_str( device, db::ID_FIELD_NAME ) );
        if( isProtectNotification( broadcastData ) ) {
            broadcastData[ "protectNotification" ] = true;
        }
    }

    addBroadcastTimestamp( broadcastData );

    std::string itemValueTypeStr = json_get_str( item, ItemFields::TypeFieldName );
    const auto valueType = Model::ValueType::fromString( itemValueTypeStr );

    Model::Item::ContainerLevel containerLevel = 0;
    for( auto type = &valueType; type && type->isContainer(); type = type->getSubType().get(), ++containerLevel ) {
        auto foundElementsMaxNumber = item.find( Model::Item::getElementsMaxNumberFieldName( containerLevel ) );
        if( foundElementsMaxNumber != item.end() ) {
            broadcastData[ getElementsMaxNumberKeyName( *type, containerLevel ) ] = foundElementsMaxNumber.value();
        }
    }

    if( item.count( ItemFields::UserCodeRestrictionFieldName ) ) {
        broadcastData[ "userCodeRestriction" ] = json_get_str( item, ItemFields::UserCodeRestrictionFieldName );
    }

    if( item.count( ItemFields::UserCodeModesFieldName ) ) {
        broadcastData[ "userCodeModes" ] = item[ ItemFields::UserCodeModesFieldName ];
    }

    if( item.count( ItemFields::StringRestrictionFieldName ) ) {
        broadcastData[ "stringRestriction" ] = json_get_str( item, ItemFields::StringRestrictionFieldName );
    }

    if( item.count( ItemFields::OneWeekDayCost ) ) {
        broadcastData[ "oneWeekDayCost" ] = item[ ItemFields::OneWeekDayCost ];
    }

    if( item.count( ItemFields::OneShiftedWeekDayCost ) ) {
        broadcastData[ "oneShiftedWeekDayCost" ] = item[ ItemFields::OneShiftedWeekDayCost ];
    }

    if( item.count( ItemFields::InfoFieldName ) ) {
        broadcastData[ "info" ] = item[ ItemFields::InfoFieldName ];
    }

    renameScalableField(
        ItemFields::ValueMinFieldName, ItemFields::Response::MinValueFieldName, itemValueTypeStr, broadcastData );
    renameScalableField(
        ItemFields::ValueMaxFieldName, ItemFields::Response::MaxValueFieldName, itemValueTypeStr, broadcastData );

    auto itemValueIt = broadcastData.find( ItemFields::ValueFieldName );
    if( itemValueIt != broadcastData.end() && valueType != Model::ValueType::Enum::NoValue ) {
        auto config = Model::ConfigurationFactory::createConfiguration( m_service.getDBClient() );
        Model::UiValueWriterVisitor valueWriter( broadcastData, *config );

        auto valueObject = Model::ValueFactory::createItemValue( valueType, *itemValueIt );
        valueWriter.writeValue( *valueObject, ItemFields::ValueFieldName );

        Model::UiValueFormattedGetterVisitor valueFormatter( *config );
        broadcastData[ ItemFields::ValueFormattedFieldName ] = Json( valueFormatter.getValue( *valueObject ) );

        if( broadcastData.find( ItemFields::TypeFieldName ) == broadcastData.end() ) {
            broadcastData[ ItemFields::TypeFieldName ] = json_get_str( item, ItemFields::TypeFieldName );
            Model::Item::decryptItemAsJson( m_provisionData, broadcastData );
            json_remove( broadcastData, ItemFields::TypeFieldName );
        }
        else {
            Model::Item::decryptItemAsJson( m_provisionData, broadcastData );
        }
    }

    return broadcastData;
}

Json& ItemDbModifier::renameScalableField( const std::string& replaceField,
                                           const std::string& responseField,
                                           const std::string& itemValueTypeStr,
                                           Json& broadcastData ) {
    using namespace Model;
    using namespace Model::Resources;

    // replace valueMin/valueMax objects with minValue/maxValue: number
    auto fieldIt = broadcastData.find( replaceField );
    if( fieldIt != broadcastData.end() ) {
        auto& serialized = fieldIt;
        auto valueTypeStr =
            itemValueTypeStr.empty() ? json_get_str( broadcastData, ItemFields::TypeFieldName ) : itemValueTypeStr;
        const auto valueType = Model::ValueType::fromString( valueTypeStr );

        bool isScalable = valueType != Model::ValueType::Enum::NoValue && valueType.getScales().size() > 0;
        if( isScalable ) {
            auto config = Model::ConfigurationFactory::createConfiguration( m_service.getDBClient() );
            auto scale = config->getScaleForValueType( valueType );
            ItemValueFloatVisitor floatVisitor( scale.c_str() );

            const auto valueObject = ValueFactory::createItemValue( valueType, broadcastData[ replaceField ] );
            valueObject->accept( floatVisitor );

            auto fetchedValue = floatVisitor.getValue();

            json_remove( broadcastData, replaceField );
            json_add_float( broadcastData, responseField, fetchedValue );
        }
    }
    return broadcastData;
}

Json& ItemDbModifier::prepareInsertBroadcastMessage( Json& broadcastData ) {
    using namespace Model::Resources;

    Model::Item::decryptItemAsJson( m_provisionData, broadcastData );

    const auto valueType = Model::ValueType::fromString( json_get_str( broadcastData, ItemFields::TypeFieldName ) );

    if( valueType != Model::ValueType::Enum::NoValue ) {
        auto config = Model::ConfigurationFactory::createConfiguration( m_service.getDBClient() );
        Model::UiValueWriterVisitor valueWriter( broadcastData, *config );

        const auto valueObject =
            Model::ValueFactory::createItemValue( valueType, broadcastData[ ItemFields::ValueFieldName ] );
        valueWriter.writeValue( *valueObject, ItemFields::ValueFieldName );

        Model::UiValueFormattedGetterVisitor fmtVisitor( *config );
        broadcastData[ ItemFields::ValueFormattedFieldName ] = fmtVisitor.getValue( *valueObject );
    }
    else {
        broadcastData[ ItemFields::ValueFormattedFieldName ] = std::string( "" );
    }

    Model::Item::ContainerLevel containerLevel = 0;
    for( auto type = &valueType; type && type->isContainer(); type = type->getSubType().get(), ++containerLevel ) {
        auto foundElementsMaxNumber =
            broadcastData.find( Model::Item::getElementsMaxNumberFieldName( containerLevel ) );
        if( foundElementsMaxNumber != broadcastData.end() ) {
            auto newKey = getElementsMaxNumberKeyName( *type, containerLevel );
            if( newKey != foundElementsMaxNumber.key() ) {
                auto value = std::move( foundElementsMaxNumber.value() );
                broadcastData.erase( foundElementsMaxNumber );

                broadcastData[ newKey ] = std::move( value );
            }
        }
    }

    renameScalableField( ItemFields::ValueMinFieldName, ItemFields::Response::MinValueFieldName, "", broadcastData );
    renameScalableField( ItemFields::ValueMaxFieldName, ItemFields::Response::MaxValueFieldName, "", broadcastData );

    broadcastData.erase( ItemFields::TypeFieldName );

    auto enumIt = broadcastData.find( ItemFields::EnumFieldName );
    if( enumIt != broadcastData.end() && enumIt->empty() ) {
        broadcastData.erase( enumIt );
    }

    broadcastData[ System::SyncNotification ] = true;

    addBroadcastTimestamp( broadcastData );

    return broadcastData;
}

Json ItemDbModifier::prepareUserNotificationBroadcastMessage( const Json& data ) {
    using namespace HubPlatform::CoreD::Model::Resources;
    const auto itemName = json_get_str( data, ItemFields::NameFieldName );
    if( itemName.empty() ) {
        return JSON_EMPTY_OBJ;
    }
    using itemValidator = Json ( * )( const IDbClient& db, const Json& data );
    static const std::map< std::string, itemValidator > itemsValidators = {
        { "user_lock_operation", &prepareUserLockOperationBroadcast },
        { "user_code_action", &prepareUserActionBroadcast },
    };

    auto itFind = itemsValidators.find( itemName );
    if( itFind == itemsValidators.end() ) {
        return JSON_EMPTY_OBJ;
    }
    return itFind->second( m_db, data );
}

bool ItemDbModifier::hasChanges() const {
    return m_changedStatus;
}

void ItemDbModifier::setChangedStatus( bool isChanged ) {
    m_changedStatus = isChanged;
}

void ItemDbModifier::sendUpdateBroadcast( MessageSubClasses subClass,
                                          Json& broadcastData,
                                          const Json& oldFieldsValues ) {
    using namespace Model::Resources;
    Json& broadcast = prepareUpdateBroadcastMessage( broadcastData, oldFieldsValues );

    broadcast = prepareHouseModeData( broadcastData, broadcast );

    if( !m_dictionary.empty() ) {
        broadcast.erase( ItemFields::ValueFieldName );
        broadcast[ DictionaryEditRequestFields::OperationFieldName ] = m_dictionary.operation;
        broadcast[ DictionaryEditRequestFields::ElementFieldName ][ m_dictionary.id ]
                 [ DictionaryEditRequestFields::ValueFieldName ] = m_dictionary.value;
        auto& oldElementObject = oldFieldsValues[ DictionaryEditRequestFields::ValueFieldName ];
        if( m_dictionary.operation == DictionaryOperations::Updated ) {
            if( !oldElementObject.empty() ) {
                auto oldElement = oldElementObject.begin();
                broadcast[ DictionaryEditRequestFields::OldElementName ] = JSON(
                    m_dictionary.id << JSON( DictionaryEditRequestFields::ValueFieldName << oldElement.value() ) );
            }
            else {
                LOGGER_ERROR( cc::error( "Empty old dictionary element" ) );
            }
        }

        DbModifier::sendUpdateBroadcast( HUB_ITEM_DICTIONARY_UPDATED, broadcast, oldFieldsValues );
    }
    else {
        auto mergeFields = []( const Json& newFields, const Json& oldValues ) {
            auto result = Json::object();
            for( auto it = newFields.begin(); it != newFields.end(); ++it ) {
                result[ it.key() ] = it.value();
            }
            for( auto it = oldValues.begin(); it != oldValues.end(); ++it ) {
                auto key = it.key();
                if( !key.empty() ) {
                    key[ 0 ] = std::toupper( key[ 0 ] );
                }
                result[ "old" + key ] = it.value();
            }
            return result;
        };
        Json merged = mergeFields( broadcast, oldFieldsValues );
        DbModifier::sendUpdateBroadcast( EXTENDED_HUB_ITEM_UPDATED, merged, oldFieldsValues );
        DbModifier::sendUpdateBroadcast( subClass, broadcast, oldFieldsValues );
    }

    JsonObject userNotificationBroadcast = prepareUserNotificationBroadcastMessage( broadcast );
    if( !userNotificationBroadcast.empty() ) {
        DbModifier::sendUpdateBroadcast( HUB_USER_NOTIFICATION, userNotificationBroadcast, oldFieldsValues );
    }
}

Json& ItemDbModifier::prepareHouseModeData( const Json& broadcastData, Json& broadcast ) {
    if( isProtectNotification( broadcastData ) ) {
        using namespace CoreDHouseModes;
        using namespace Resources;
        const auto mode = getCurrentHouseMode( m_db );
        const auto ItemId = json_get_str( broadcastData, Model::Resources::ItemFields::IdFieldName );
        auto modeArmed = mode->m_armed;
        auto modeId = mode->m_id;
        auto alarmDelay = getAlarmDelay( getDBClient() );

        broadcast[ "houseMode" ] = JSON( "id" << modeId << "armed" << modeArmed << "alarmDelay" << alarmDelay );
    }
    return broadcast;
}

void ItemDbModifier::updateDictionary( const IDbModifier::Id& recordId,
                                       const Json& updatePortion,
                                       const Dictionary& dictionary ) {
    m_dictionary = dictionary;
    update( recordId, updatePortion );
    m_dictionary.clear();
}

bool ItemDbModifier::shouldUserBeNotified( const std::string& itemName,
                                           const std::string& deviceCategory,
                                           const std::string& deviceSubcategory,
                                           const Json& broadcastData,
                                           const Json& oldFieldsValues ) {
    using itemValidator = bool ( * )( const std::string& deviceCategory,
                                      const std::string& deviceSubcategory,
                                      const Json& broadcastData,
                                      const Json& oldFieldsValues );
    static const std::map< std::string, itemValidator > itemsValidators = {
        { "battery", &shouldUserBeNotifiedForBatteryItem },
        { "security_threat", &shouldUserBeNotifiedForSecurityThreatItem },
        { "smoke_alarm", &shouldUserBeNotifiedForSmokeItems },
        { "lock_operation", &shouldUserBeNotifiedForLockOperationItems },
        { "tampering_cover_alarm", &shouldUserBeNotifiedForTamperingCoverAlarmItems },
        { "dw_state", &shouldUserBeNotifiedForDwStateItems },
        { "water_leak_alarm", &shouldUserBeNotifiedForWaterLeakAlarmItems },
        { "motion", &shouldUserBeNotifiedForMotionItems },
        { "co_alarm", &shouldUserBeNotifiedCoAlarmItems },
        { "glass_breakage_alarm", &shouldUserBeNotifiedGlassBreakageAlarmItems },
        { "user_code_operation", &shouldUserBeNotifiedUserCodeOperationItems },
        { "thermostat_mode", &shouldUserBeNotifiedThermostatModeItems },
        { "thermostat_fan_mode", &shouldUserBeNotifiedThermostatFanModeItems },
        { "switch", &shouldUserBeNotifiedSwitchItems },
        { "barrier_state", &shouldUserBeNotifiedBarrierStateItems },
        { "water_temperature_alarm", &shouldUserBeNotifiedWaterTemperatureAlarmItems },
        { "heat_alarm", &shouldUserBeNotifiedHeatAlarmItems },
        { "doorbell_state", &shouldUserBeNotifiedDoorBellRing },
        { "hw_state", &notifyUserAboutHardwareState },
        { "sw_state", &notifyUserAboutSoftwareState },
        { "position", &notifyUserAboutHealthPositionChanged },
        { "siren_alarm", &notifyUserAboutSirenState }
    };

    auto itFind = itemsValidators.find( itemName );
    if( itFind == itemsValidators.end() ) {
        for( const auto& modifier : m_modifyBroadcast ) {
            if( modifier->shouldUserBeNotified( itemName, deviceCategory, deviceSubcategory, broadcastData ) )
                return true;
        }
        return false;
    }
    return itFind->second( deviceCategory, deviceSubcategory, broadcastData, oldFieldsValues );
}

bool ItemDbModifier::shouldUserBeNotifiedForBatteryItem( const std::string& /*deviceCategory*/,
                                                         const std::string& /*deviceSubcategory*/,
                                                         const Json& broadcastData,
                                                         const Json& /*oldFieldsValues*/ ) {
    std::int32_t batteryLevel = 0;
    if( json_get( broadcastData, "value", batteryLevel ) ) {
        return batteryLevel <= 20;
    }
    return false;
}

bool ItemDbModifier::shouldUserBeNotifiedForSecurityThreatItem( const std::string& deviceCategory,
                                                                const std::string& deviceSubcategory,
                                                                const Json& broadcastData,
                                                                const Json& /*oldFieldsValues*/ ) {
    auto shouldUserBeNotified = [&broadcastData]() {
        bool isTriggered = false;
        bool deviceArmed = false;
        return json_get( broadcastData, "value", isTriggered ) &&
               json_get( broadcastData, "deviceArmed", deviceArmed ) && isTriggered && deviceArmed;
    };
    if( deviceCategory == "security_sensor" ) {
        if( deviceSubcategory == "door" ) {
            return shouldUserBeNotified();
        }
        else if( deviceSubcategory == "leak" ) {
            return shouldUserBeNotified();
        }
        else if( deviceSubcategory == "motion" ) {
            return shouldUserBeNotified();
        }
    }
    else if( deviceCategory == "scene_controller" ) {
        if( deviceSubcategory == "panic" ) {
            return shouldUserBeNotified();
        }
    }
    return false;
}

bool ItemDbModifier::shouldUserBeNotifiedForSmokeItems( const std::string& /*deviceCategory*/,
                                                        const std::string& /*deviceSubcategory*/,
                                                        const Json& broadcastData,
                                                        const Json& oldFieldsValues ) {
    std::string alarmState;
    if( json_get( broadcastData, "value", alarmState ) ) {
        if( alarmState == "smoke_detected" ) {
            return true;
        }
        bool deviceArmed;
        std::string oldAlarmState;
        if( json_get( broadcastData, "deviceArmed", deviceArmed ) &&
            json_get( oldFieldsValues, "value", oldAlarmState ) ) {
            return alarmState == "no_smoke" && oldAlarmState != "unknown" && deviceArmed;
        }
    }
    return false;
}

bool ItemDbModifier::shouldUserBeNotifiedForLockOperationItems( const std::string& /*deviceCategory*/,
                                                                const std::string& /*deviceSubcategory*/,
                                                                const Json& broadcastData,
                                                                const Json& /*oldFieldsValues*/ ) {
    std::string state;
    if( json_get( broadcastData, "value", state ) ) {
        return state == "unlock_by_rf_with_invalid_user_code" || state == "lock_by_rf_with_invalid_user_code" ||
               state == "manual_lock_operation" || state == "keypad_lock_operation" ||
               state == "auto_lock_not_fully_locked_operation" || state == "rf_not_fully_locked_operation" ||
               state == "manual_not_fully_locked_operation" || state == "lock_jammed" || state == "rf_lock_operation" ||
               state == "manual_unlock_operation" || state == "rf_unlock_operation" ||
               state == "keypad_unlock_operation";
    }
    return false;
}

bool ItemDbModifier::shouldUserBeNotifiedForTamperingCoverAlarmItems( const std::string& /*deviceCategory*/,
                                                                      const std::string& /*deviceSubcategory*/,
                                                                      const Json& broadcastData,
                                                                      const Json& oldFieldsValues ) {
    std::string state;
    std::string oldState;
    if( json_get( broadcastData, "value", state ) && json_get( oldFieldsValues, "value", oldState ) ) {
        return ( state == "no_tampering_cover" && oldState != "unknown" ) || state == "tampering_cover";
    }
    return false;
}

bool ItemDbModifier::shouldUserBeNotifiedForDwStateItems( const std::string& /*deviceCategory*/,
                                                          const std::string& /*deviceSubcategory*/,
                                                          const Json& broadcastData,
                                                          const Json& oldFieldsValues ) {
    std::string state;
    std::string oldState;
    bool deviceArmed;
    if( json_get( broadcastData, "value", state ) && json_get( oldFieldsValues, "value", oldState ) &&
        json_get( broadcastData, "deviceArmed", deviceArmed ) ) {
        return state == "dw_is_closed" && oldState != "unknown" && deviceArmed;
    }
    return false;
}

bool ItemDbModifier::shouldUserBeNotifiedForWaterLeakAlarmItems( const std::string& /*deviceCategory*/,
                                                                 const std::string& /*deviceSubcategory*/,
                                                                 const Json& broadcastData,
                                                                 const Json& oldFieldsValues ) {
    std::string state;
    std::string oldState;
    if( json_get( broadcastData, "value", state ) && json_get( oldFieldsValues, "value", oldState ) ) {
        bool deviceArmed;
        if( json_get( broadcastData, "deviceArmed", deviceArmed ) ) {
            return state == "no_water_leak" && oldState != "unknown" && deviceArmed;
        }
    }
    return false;
}

bool ItemDbModifier::shouldUserBeNotifiedForMotionItems( const std::string& /*deviceCategory*/,
                                                         const std::string& /*deviceSubcategory*/,
                                                         const Json& broadcastData,
                                                         const Json& /*oldFieldsValues*/ ) {
    bool state;
    if( json_get( broadcastData, "value", state ) ) {
        bool deviceArmed;
        if( json_get( broadcastData, "deviceArmed", deviceArmed ) ) {
            return state == false && deviceArmed;
        }
    }
    return false;
}

bool ItemDbModifier::shouldUserBeNotifiedCoAlarmItems( const std::string& /*deviceCategory*/,
                                                       const std::string& /*deviceSubcategory*/,
                                                       const Json& broadcastData,
                                                       const Json& oldFieldsValues ) {
    std::string state;
    std::string oldState;
    if( json_get( broadcastData, "value", state ) && json_get( oldFieldsValues, "value", oldState ) ) {
        bool deviceArmed;
        if( json_get( broadcastData, "deviceArmed", deviceArmed ) ) {
            return ( state == "co_detected" || ( state == "no_co" && oldState != "unknown" ) ) && deviceArmed;
        }
    }
    return false;
}

bool ItemDbModifier::shouldUserBeNotifiedGlassBreakageAlarmItems( const std::string& /*deviceCategory*/,
                                                                  const std::string& /*deviceSubcategory*/,
                                                                  const Json& broadcastData,
                                                                  const Json& /*oldFieldsValues*/ ) {
    std::string state;
    if( json_get( broadcastData, "value", state ) ) {
        bool deviceArmed;
        if( json_get( broadcastData, "deviceArmed", deviceArmed ) ) {
            return state == "glass_breakage" && deviceArmed;
        }
    }
    return false;
}

bool ItemDbModifier::shouldUserBeNotifiedUserCodeOperationItems( const std::string& /*deviceCategory*/,
                                                                 const std::string& /*deviceSubcategory*/,
                                                                 const Json& broadcastData,
                                                                 const Json& /*oldFieldsValues*/ ) {
    std::string state;
    if( json_get( broadcastData, "value", state ) ) {
        return state == "new_program_code_entered" || state == "new_user_code_not_added_due_to_duplicated_code" ||
               state == "manually_enter_user_code_exceeds_code_limit" || state == "new_user_code_added" ||
               state == "single_user_code_deleted" || state == "all_user_codes_deleted";
    }
    return false;
}

bool ItemDbModifier::shouldUserBeNotifiedThermostatModeItems( const std::string& /*deviceCategory*/,
                                                              const std::string& /*deviceSubcategory*/,
                                                              const Json& broadcastData,
                                                              const Json& /*oldFieldsValues*/ ) {
    std::string state;
    if( json_get( broadcastData, "value", state ) ) {
        return state == "off" || state == "heat" || state == "cool" || state == "auto";
    }
    return false;
}

bool ItemDbModifier::shouldUserBeNotifiedThermostatFanModeItems( const std::string& /*deviceCategory*/,
                                                                 const std::string& /*deviceSubcategory*/,
                                                                 const Json& broadcastData,
                                                                 const Json& /*oldFieldsValues*/ ) {
    std::string state;
    if( json_get( broadcastData, "value", state ) ) {
        return state == "low" || state == "high" || state == "medium" || state == "auto_low" || state == "auto_high" ||
               state == "auto_medium";
    }
    return false;
}

bool ItemDbModifier::shouldUserBeNotifiedSwitchItems( const std::string& /*deviceCategory*/,
                                                      const std::string& /*deviceSubcategory*/,
                                                      const Json& broadcastData,
                                                      const Json& /*oldFieldsValues*/ ) {
    bool state;
    if( json_get( broadcastData, "value", state ) ) {
        return state == true || state == false;
    }
    return false;
}

bool ItemDbModifier::shouldUserBeNotifiedBarrierStateItems( const std::string& /*deviceCategory*/,
                                                            const std::string& /*deviceSubcategory*/,
                                                            const Json& broadcastData,
                                                            const Json& /*oldFieldsValues*/ ) {
    std::string state;
    if( json_get( broadcastData, "value", state ) ) {
        return state == "barrier_opened" || state == "barrier_closed";
    }
    return false;
}

bool ItemDbModifier::shouldUserBeNotifiedWaterTemperatureAlarmItems( const std::string& /*deviceCategory*/,
                                                                     const std::string& /*deviceSubcategory*/,
                                                                     const Json& broadcastData,
                                                                     const Json& /*oldFieldsValues*/ ) {
    std::string state;
    if( json_get( broadcastData, "value", state ) ) {
        return state == "water_temperature_below_low_threshold" || state == "water_temperature_above_high_threshold";
    }
    return false;
}

bool ItemDbModifier::shouldUserBeNotifiedHeatAlarmItems( const std::string& deviceCategory,
                                                         const std::string& deviceSubcategory,
                                                         const Json& broadcastData,
                                                         const Json& /*oldFieldsValues*/ ) {
    if( json_get_bool( broadcastData, "deviceArmed", false ) ) {
        std::string state;
        if( json_get( broadcastData, "value", state ) && !state.empty() ) {
            if( "overheat_detected" == state || "under_heat_detected" == state ) {
                return "heat" == deviceSubcategory && "security_sensor" == deviceCategory;
            }
        }
    }
    return false;
}

bool ItemDbModifier::shouldUserBeNotifiedDoorBellRing( const std::string& /*deviceCategory*/,
                                                       const std::string& /*deviceSubcategory*/,
                                                       const Json& broadcastData,
                                                       const Json& /*oldFieldsValues*/ ) {
    return json_get_str( broadcastData, "value" ) == "ring";
}

bool ItemDbModifier::notifyUserAboutHardwareState( const std::string& /*deviceCategory*/,
                                                   const std::string& /*deviceSubcategory*/,
                                                   const Json& broadcastData,
                                                   const Json& oldFieldsValues ) {
    std::string state;
    std::string oldState;
    if( json_get( broadcastData, "value", state ) && json_get( oldFieldsValues, "value", oldState ) ) {
        return state == "hardware_failure" || ( state == "hardware_ok" && oldState != "unknown" );
    }

    return false;
}

bool ItemDbModifier::notifyUserAboutSoftwareState( const std::string& /*deviceCategory*/,
                                                   const std::string& /*deviceSubcategory*/,
                                                   const Json& broadcastData,
                                                   const Json& oldFieldsValues ) {
    std::string state;
    std::string oldState;
    if( json_get( broadcastData, "value", state ) && json_get( oldFieldsValues, "value", oldState ) ) {
        return state == "software_failure" || ( state == "software_ok" && oldState != "unknown" );
    }

    return false;
}

bool ItemDbModifier::notifyUserAboutHealthPositionChanged( const std::string& /*deviceCategory*/,
                                                           const std::string& /*deviceSubcategory*/,
                                                           const Json& broadcastData,
                                                           const Json& /*oldFieldsValues*/ ) {
    auto state = json_get_str( broadcastData, "value" );
    return state == "leaving_bed" || state == "sitting_on_bed" || state == "lying_on_bed" ||
           state == "sitting_on_bed_edge";
}

bool ItemDbModifier::notifyUserAboutSirenState( const std::string& deviceCategory,
                                                const std::string& /*deviceSubcategory*/,
                                                const Json& broadcastData,
                                                const Json& /*oldFieldsValues*/ ) {
    auto state = json_get_str( broadcastData, "value" );
    return deviceCategory == "siren" && ( state == "siren_active" || state == "siren_inactive" );
}

Json ItemDbModifier::prepareUserLockOperationBroadcast( const IDbClient& db, const Json& data ) {
    using namespace HubPlatform::CoreD::Model::Resources;
    using namespace JsonCustomGetters;

    struct {
        int userId = 0;
        std::string action;
    } lockOperation;

    const auto lockOperationGetter = makeFieldGetter(
        ItemFields::ValueFieldName,
        makeStructGetter(
            makeMemberMapping( &decltype( lockOperation )::userId, UserLockOperation::Fields::UserId ),
            makeMemberMapping( &decltype( lockOperation )::action, UserLockOperation::Fields::Action ) ) );

    if( !lockOperationGetter( data, lockOperation ) ) {
        LOGGER_WARN( "Invalid lock operation value " + cc::j( data ) );
        return {};
    }

    if( lockOperation.action == "unknown" ) {
        LOGGER_TRACE( "Unknown lock operation action" );
        return {};
    }

    return generatePinCodeEnteredBroadcast( db, data, lockOperation.userId );
}

Json ItemDbModifier::prepareUserActionBroadcast( const IDbClient& db, const Json& data ) {
    using namespace HubPlatform::CoreD::Model::Resources;
    using namespace JsonCustomGetters;

    struct {
        int userId = 0;
        std::string action;
    } userCodeAction;

    const auto actionGetter = makeFieldGetter(
        ItemFields::ValueFieldName,
        makeStructGetter( makeMemberMapping( &decltype( userCodeAction )::userId, UserCodeAction::Fields::UserId ),
                          makeMemberMapping( &decltype( userCodeAction )::action, UserCodeAction::Fields::Action ) ) );

    if( !actionGetter( data, userCodeAction ) ) {
        LOGGER_WARN( "Invalid user code action value " + cc::j( data ) );
        return {};
    }

    if( userCodeAction.action == "unknown" ) {
        LOGGER_TRACE( "Unknown user code action" );
        return {};
    }

    if( userCodeAction.action == "invalid_user_code_entered" ) {
        LOGGER_TRACE( "Invalid user code entered" );
        return {};
    }

    return generatePinCodeEnteredBroadcast( db, data, userCodeAction.userId );
}

Json ItemDbModifier::generatePinCodeEnteredBroadcast( const IDbClient& db, const Json& data, int userId ) {
    using namespace HubPlatform::CoreD::Model::Resources;

    JsonObject broadcastData = JSON_EMPTY_OBJ;
    broadcastData[ "type" ] = "pin_code_is_used";
    JsonObject& params = broadcastData[ "params" ];
    params[ "deviceName" ] = data[ "deviceName" ];
    params[ "roomName" ] = data[ "roomName" ];
    params[ "user" ] = JSON_EMPTY_OBJ;
    params[ "user" ] = "Code " + std::to_string( userId );
    std::string deviceId;
    if( !json_get( data, ItemFields::DeviceIdFieldName, deviceId ) ) {
        LOGGER_TRACE( "Empty deviceId" );
        return broadcastData;
    }
    JsonObject fieldsToReturn = JSON( ItemFields::IdFieldName << 1 << ItemFields::ValueFieldName << 1 );
    auto value =
        db.findOne( DBNames::ItemsDB,
                    JSON( ItemFields::DeviceIdFieldName << deviceId << ItemFields::NameFieldName << "user_codes" ),
                    &fieldsToReturn );
    auto dynamicValue = db.findOne(
        DBNames::TempItemsDB, JSON( ItemFields::IdFieldName << value[ ItemFields::IdFieldName ] ), &fieldsToReturn );

    value = JSON( value << dynamicValue );
    LOGGER_TRACE( value.dump() );
    if( value.empty() ) {
        LOGGER_TRACE( "Empty value" );
        return broadcastData;
    }
    auto codesIt = value.find( ItemFields::ValueFieldName );
    if( codesIt == value.end() ) {
        LOGGER_TRACE( "Empty codesIt" );
        return broadcastData;
    }
    std::string userName;
    const JsonArray& codes = *codesIt;
    auto codeIt = codes.find( std::to_string( userId ) );
    if( codeIt == codes.end() || !json_get( *codeIt, "name", userName ) ) {
        LOGGER_TRACE( "No name" );
        return broadcastData;
    }
    params[ "user" ] = userName;
    return broadcastData;
}

Json& ItemDbModifier::prepareRemoveBroadcastMessage( Json& broadcastData, const Json& removedRecord ) {
    const std::string deviceIdField = Model::Resources::ItemFields::DeviceIdFieldName;
    if( removedRecord.is_object() && removedRecord.count( deviceIdField ) > 0 ) {
        broadcastData[ deviceIdField ] = removedRecord.at( deviceIdField );
    }
    else {
        LOGGER_TRACE( "removedRecord is not an object or has no deviceId field: " + cc::j( removedRecord ) );
    }
    addBroadcastTimestamp( broadcastData );
    return broadcastData;
}

bool ItemDbModifier::sendEzloProtectNotification( const CoreDHouseModes::HouseModeEntry& currentHouseMode,
                                                  const std::string& deviceId ) const {
    const bool bypassDevice = currentHouseMode.m_bypassDeviceIds.count( deviceId ) > 0;
    const bool protect = currentHouseMode.m_protect;
    return protect && !bypassDevice && m_featureManager.getFeatureStatus( "protect" );
}

bool ItemDbModifier::isProtectNotification( const Json& updateNotification ) {
    if( json_get_str( updateNotification, "name" ) == "smoke_alarm" &&
        json_get_str( updateNotification, "value" ) == "smoke_detected" ) {
        return true;
    }

    if( json_get_str( updateNotification, "name" ) == "co_alarm" &&
        json_get_str( updateNotification, "value" ) == "co_detected" &&
        json_get_bool( updateNotification, "deviceArmed", false ) ) {
        return true;
    }

    if( json_get_str( updateNotification, "name" ) == "glass_breakage_alarm" &&
        json_get_str( updateNotification, "value" ) == "glass_breakage" &&
        json_get_bool( updateNotification, "deviceArmed", false ) ) {
        return true;
    }

    if( json_get_str( updateNotification, "name" ) == "security_threat" &&
        json_get_bool( updateNotification, "value", false ) &&
        json_get_str( updateNotification, "deviceCategory" ) == "security_sensor" &&
        json_get_str( updateNotification, "deviceSubcategory" ) == "door" &&
        json_get_bool( updateNotification, "deviceArmed", false ) ) {
        return true;
    }

    if( json_get_str( updateNotification, "name" ) == "security_threat" &&
        json_get_bool( updateNotification, "value", false ) &&
        json_get_str( updateNotification, "deviceCategory" ) == "security_sensor" &&
        json_get_str( updateNotification, "deviceSubcategory" ) == "motion" &&
        json_get_bool( updateNotification, "deviceArmed", false ) ) {
        return true;
    }

    if( json_get_str( updateNotification, "name" ) == "security_threat" &&
        json_get_bool( updateNotification, "value", false ) &&
        json_get_str( updateNotification, "deviceCategory" ) == "scene_controller" &&
        json_get_str( updateNotification, "deviceSubcategory" ) == "panic" &&
        json_get_bool( updateNotification, "deviceArmed", false ) &&
        !json_get_str( updateNotification, "protectService" ).empty() ) {
        return true;
    }

    if( json_get_bool( updateNotification, "userNotification", false ) ) {
        auto deviceId = json_get_str( updateNotification, "deviceId" );
        auto dbFilter = JSON( db::ID_FIELD_NAME << deviceId );
        auto foundIds = CoreDHouseModes::getProtectDevicesIds( m_service.getDBClient(), dbFilter );
        if( !foundIds.empty() )
            return true;
    }

    return false;
}

bool ItemDbModifier::prepareProtectServiceField( Json& broadcast, const std::string& deviceId ) {
    if( !deviceId.empty() && json_get_bool( broadcast, "deviceArmed", false ) &&
        json_get_str( broadcast, "deviceSubcategory" ) == "panic" &&
        json_get_str( broadcast, "deviceCategory" ) == "scene_controller" &&
        json_get_str( broadcast, "name" ) == "security_threat" ) {
        auto findResult = CoreDHouseModes::findProtectButtonById( getDBClient(), deviceId );
        if( findResult.second ) {
            json_add_str( broadcast, "protectService", CoreDHouseModes::toString( findResult.first.service() ) );
            return true;
        }
    }
    return false;
}

}  // namespace CoreD
}  // namespace HubPlatform
