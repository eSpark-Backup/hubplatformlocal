#include "cored/db_modifier/device_db_modifier.h"
#include "cored/model/resources/cored_resources.h"

#include "database_client/dbqueryparser.h"
#include "database_client/idbclient.h"

#include <stdexcept>

namespace HubPlatform {
namespace CoreD {

class DbModifierError : public std::runtime_error {
public:
    explicit DbModifierError( const std::string& errDescription )
    : runtime_error( "DbModifier: " + errDescription ) {}
};

DeviceDbModifier::DeviceDbModifier( IDbClient& db, Broadcaster broadcaster )
: DbModifier( db,
              std::move( broadcaster ),
              Model::Resources::DBNames::DevicesDB,
              { { HUB_DEVICE_ADDED, {} }, { HUB_DEVICE_UPDATED, {} }, { HUB_DEVICE_REMOVED, {} } },
              { Model::Resources::DeviceFields::ReachableFieldName, Model::Resources::DeviceFields::ReadyFieldName } )
, m_excludeBroadcastKeys( { Model::Resources::DeviceFields::HouseModesOptionsFieldName } ) {}

bool DeviceDbModifier::hasChanges() const {
    return m_changedStatus;
}

void DeviceDbModifier::setChangedStatus( bool isChanged ) {
    m_changedStatus = isChanged;
}

namespace {
Json& removeInternalFields( Json& broadcastData, const std::set< std::string >& excludeKeys ) {
    for( const auto& key : excludeKeys ) {
        try {
            broadcastData.erase( key );
        }
        catch( const std::exception& ) {
        }
    }
    return broadcastData;
}
}  // namespace

Json& DeviceDbModifier::prepareInsertBroadcastMessage( Json& broadcastData ) {
    removeInternalFields( broadcastData, m_excludeBroadcastKeys );
    addBroadcastTimestamp( broadcastData );
    return DbModifier::prepareInsertBroadcastMessage( broadcastData );
}

Json& DeviceDbModifier::prepareUpdateBroadcastMessage( Json& broadcastData ) {
    using namespace Model::Resources;
    const auto device =
        m_db.findOne( m_dbName, JSON( db::ID_FIELD_NAME << broadcastData[ db::ID_FIELD_NAME ] ), nullptr );
    bool serviceNotificationFlag;
    if( json_get( device, DeviceFields::ServiceNotificationFieldName, serviceNotificationFlag ) ) {
        broadcastData[ DeviceFields::ServiceNotificationFieldName ] = serviceNotificationFlag;
    }
    broadcastData[ System::SyncNotification ] = json_contains( broadcastData, DeviceFields::NameFieldName ) ||
                                                json_contains( broadcastData, DeviceFields::FirmwareFieldName ) ||
                                                json_contains( broadcastData, DeviceFields::RoomIdFieldName );
    removeInternalFields( broadcastData, m_excludeBroadcastKeys );
    addBroadcastTimestamp( broadcastData );
    return broadcastData;
}

Json& DeviceDbModifier::prepareRemoveBroadcastMessage( Json& broadcastData, const Json& removedRecord ) {
    using namespace Model::Resources;
    bool serviceNotificationFlag;
    if( json_get( removedRecord, DeviceFields::ServiceNotificationFieldName, serviceNotificationFlag ) ) {
        broadcastData[ DeviceFields::ServiceNotificationFieldName ] = serviceNotificationFlag;
    }
    broadcastData[ System::SyncNotification ] = true;
    addBroadcastTimestamp( broadcastData );
    return broadcastData;
}

void DeviceDbModifier::sendUpdateBroadcast( MessageSubClasses subClass,
                                            Json& broadcastData,
                                            const Json& oldFieldsValues ) {
    DbModifier::sendUpdateBroadcast( subClass, prepareUpdateBroadcastMessage( broadcastData ), oldFieldsValues );
}

bool DeviceDbModifier::isUpdateBroadcastEmpty( const Json& broadcastData ) const {
    if( !m_excludeBroadcastKeys.empty() && broadcastData.is_object() && !broadcastData.empty() ) {
        int excludedKeys = 0;
        auto countExcludedKeys = [&excludedKeys, &broadcastData]( const std::string& key ) {
            if( broadcastData.find( key ) != broadcastData.end() )
                ++excludedKeys;
        };
        std::for_each( m_excludeBroadcastKeys.begin(), m_excludeBroadcastKeys.end(), countExcludedKeys );

        bool hasOnlyInternalFields = excludedKeys >= broadcastData.size();
        if( hasOnlyInternalFields )
            return true;
    }
    return DbModifier::isUpdateBroadcastEmpty( broadcastData );
}

}  // namespace CoreD
}  // namespace HubPlatform
