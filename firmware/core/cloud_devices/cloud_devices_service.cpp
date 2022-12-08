#include "cloud_devices/cloud_devices_service.h"
#include "cloud_devices/resources/cloud_devices_resources.h"

#if defined( __WITH_CLOUD_DEVICES_LEGACY_CODE__ )
#include "cloud_devices/script_events/command_response.h"
#include "cloud_devices/script_events/device_added.h"
#include "cloud_devices/script_events/device_updated.h"
#include "cloud_devices/script_events/device_removed.h"
#endif  // __WITH_CLOUD_DEVICES_LEGACY_CODE__

#include "uid/remote_interface/ui_remote_interface.h"
#include "cored/remote_interface/cored_interface_factory.h"
#include "service_interfaces/bus_wrapper.h"
#include "system_utils/environment.h"
#include "common_utils/backup/backup_helper.h"
#include "common_utils/backup/restore_helper.h"
#include "common_utils/lock_helper.h"
#include "common_utils/cpp14.h"
#include "json_utils/json_utils.h"
#include "json_utils/json_wrapper.h"
#include "jsonrpc/jsonrpc.h"

#include <string>

namespace HubPlatform {
namespace CloudDevices {

const char* const CloudDevicesService::Name = "cloud_devices";
namespace {
const auto CurrentApiVersion = "1";
const auto DefaultCloudRequestTimeout = std::chrono::seconds( 30 );
const auto DefaultReaskOnErrorDelay = std::chrono::minutes( 10 );
}  // namespace

CloudDevicesService::CloudDevicesService()
: Services::Service( Name )
, m_cloudRequestTimeout( DefaultCloudRequestTimeout )
, m_reaskOnErrorDelay( DefaultReaskOnErrorDelay )
, m_pendingControllerAbstractsList( false )
, m_updateControllerAbstractsList( false ) {}

CloudDevicesService::CloudDevicesService( const std::string& name,
                                          std::unique_ptr< Services::BusWrapper > impl,
                                          std::unique_ptr< IDbClient > dbClient )
: Services::Service( name, std::move( impl ), std::move( dbClient ) )
, m_pendingControllerAbstractsList( false )
, m_updateControllerAbstractsList( false ) {}

void CloudDevicesService::setUp() {
    Services::Service::setUp();

#if defined( __WITH_CLOUD_DEVICES_LEGACY_CODE__ )
    if( m_configReader.hasValue( Resources::ConfigKeys::CloudRequestTimeout ) ) {
        m_cloudRequestTimeout =
            std::chrono::seconds( m_configReader.getInt( Resources::ConfigKeys::CloudRequestTimeout ) );
    }

    if( m_configReader.hasValue( Resources::ConfigKeys::ReaskOnErrorDelay ) ) {
        m_reaskOnErrorDelay = std::chrono::seconds( m_configReader.getInt( Resources::ConfigKeys::ReaskOnErrorDelay ) );
    }
#endif  // __WITH_CLOUD_DEVICES_LEGACY_CODE__

    m_core = CoreDRemoteInterface::makeInstance( *this );
    m_subscriptionHelper = std::make_unique< SubscriptionHelper >( *this, *m_core );
    m_subscriptionManager = std::make_unique< SubscriptionManager >( *m_subscriptionHelper, getDBClient() );

    join( GroupsNames::CLOUD_GROUP );
    join( GroupsNames::CLOUD_DEVICES );

    if( Uid::RemoteInterface( *this ).isRegisteredOnCloud() ) {
        handleCloudRegistered();
    }
    else {
        handleCloudUnregistered();
    }
}

void CloudDevicesService::tearDown() {
    m_subscriptionHelper->tearDown();
    Services::Service::tearDown();
}

void CloudDevicesService::handleMessageFromBus( const Services::Message& message, const std::string& sender ) {
    switch( message.msgClass ) {
    case EVENT:
        if( handleEventFromBus( message ) ) {
            break;
        }
    default:
        Services::Service::handleMessageFromBus( message, sender );
        break;
    }
}

JsonObject CloudDevicesService::handleRequestFromBus( const JsonObject& request, MessageSubClasses requestType ) {
    switch( requestType ) {
    case CLOUD_MESSAGE: {
        return unhandledRequestAnswer();
    }
    case GLOBAL_LOCK: {
        // NOTE: cloud devices/items updates in DB will not be done during this service backup
        //       and will not make system de-synced during other services backup
        // NOTE: Nothing to do here

        auto& lockHelper = CommonUtils::LockHelper::global();
        lockHelper.tryLock( request );
        return lockHelper.getResponse();
    }
    case GLOBAL_UNLOCK: {
        auto& lockHelper = CommonUtils::LockHelper::global();
        lockHelper.unlock( request );
        return lockHelper.getResponse();
    }
    case BACKUP_CREATE: {
        CommonUtils::BackupHelper helper( getDBClient(), request );
        for( auto& tableName : Resources::DB::BackupTableNames ) {
            if( !helper.backupDatabase( tableName ) ) {
                return helper.getResponse();
            }
        }
        return helper.getResponse();
    }
    case HUB_CLOUD_SUBSCRIPTION_CREATE:
        return handleCloudSubscriptionCreate( request );
    case HUB_CLOUD_SUBSCRIPTION_UPDATE:
        return handleCloudSubscriptionUpdate( request );
    case HUB_CLOUD_SUBSCRIPTION_DELETE:
        return handleCloudSubscriptionDelete( request );
    default:
        return Service::handleRequestFromBus( request, requestType );
    }
    return JSON_EMPTY_OBJ;
}

int CloudDevicesService::restoreBackup( const std::string& tempBackupFolder ) {
    CommonUtils::RestoreHelper helper( getDBClientSafe(), tempBackupFolder );
    for( auto& tableName : Resources::DB::BackupTableNames ) {
        if( !helper.restoreDatabase( tableName ) ) {
            return helper.getReturnCode();
        }
    }
    return helper.getReturnCode();
}

void CloudDevicesService::handleLegacyMethodCall( const JsonObject& json, const std::string& sender ) {
    Services::Service::handleLegacyMethodCall( json, sender );
}

void CloudDevicesService::handleMethodCall( const JsonObject& json,
                                            MessageSubClasses requestType,
                                            const std::string& /*sender*/ ) {
    using MethodHandler = void ( CloudDevicesService::* )( const JsonObject& json );
    using MethodHandlersMap = std::unordered_map< MessageSubClasses, MethodHandler, MessageSubClassesHash >;

    static const MethodHandlersMap methodHandlers = {
#if defined( __WITH_CLOUD_DEVICES_LEGACY_CODE__ )
        { CLOUD_DEVICES_SEND_COMMAND, &CloudDevicesService::handleSendCommandRequest },
#endif
        { CLOUD_DEVICES_GATEWAY_COMMAND, &CloudDevicesService::handleGatewayCommandRequest },
    };

    const auto it = methodHandlers.find( requestType );
    if( it != methodHandlers.end() ) {
        const auto handler = it->second;
        ( this->*handler )( json );
    }
}

bool CloudDevicesService::handleEventFromBus( const Services::Message& message ) {
    switch( message.msgSubClass ) {
    case CLOUD_REGISTERED:
        handleCloudRegistered();
        return true;
    case CLOUD_UNREGISTERED:
        handleCloudUnregistered();
        return true;
    case CLOUD_MESSAGE:
        handleCloudEventFromBus( message );
        return true;
    default:
        return false;
    }
}

void CloudDevicesService::handleCloudEventFromBus( const Services::Message& message ) {
#if defined( __WITH_CLOUD_DEVICES_LEGACY_CODE__ )
    auto msgSubclass = json_get_str( message.data, "msg_subclass" );
    const auto& joResult = message.data[ "result" ];
    if( msgSubclass == "cloud.abstracts.updated" ) {
        handleCloudAbstractsUpdated( joResult );
    }
    else if( msgSubclass == "cloud.abstracts.added" ) {
        handleCloudAbstractsAdded( joResult );
    }
    else if( msgSubclass == "cloud.abstracts.deleted" ) {
        handleCloudAbstractsDeleted( joResult );
    }
#endif  // __WITH_CLOUD_DEVICES_LEGACY_CODE__
}

#if defined( __WITH_CLOUD_DEVICES_LEGACY_CODE__ )
int CloudDevicesService::getExistingDevicesDataVersion() const {
    using namespace Resources::DB;
    auto result = getDBClient().findOne( TableNames::Versions, JSON( Fields::Id << Values::DevicesTag ), nullptr );
    return json_get_int( result, Fields::Version, -1 );
}

bool CloudDevicesService::setCurrentDevicesDataVersion( int version ) {
    using namespace Resources::DB;
    getDBClient().update( TableNames::Versions,
                          JSON_EMPTY_OBJ,
                          JSON( Fields::Id << Values::DevicesTag << Fields::Version << version ),
                          true,
                          false );
    return true;
}

void CloudDevicesService::requestControllerAbstractsList() {
    using namespace Resources;
    if( m_pendingControllerAbstractsList ) {
        Log::debug( cc::debug( "Scheduling re-checking cloud devices list" ) );
        m_updateControllerAbstractsList = true;
        return;
    }
    int currentVersion = getExistingDevicesDataVersion();
    Log::info( cc::info( "Checking for new cloud devices list (current v" ) +
               cc::note( std::to_string( currentVersion ) ) + cc::info( ")" ) );
    m_updateControllerAbstractsList = false;
    m_pendingControllerAbstractsList = true;

    Json params;
    params[ API::DevicesList::Version ] = currentVersion;
    Json message;
    message[ API::General::Api ] = CurrentApiVersion;
    message[ API::General::Method ] = "cloud.controller_abstracts_list";
    message[ API::General::Params ] = params;
    sendAsyncRequest(
        GroupsNames::UI_GROUP,
        message,
        CLOUD_MESSAGE,
        std::bind( &CloudDevicesService::handleControllerAbstractsListResponse, this, std::placeholders::_1 ),
        std::bind( &CloudDevicesService::handleControllerAbstractsListTimeout, this ),
        m_cloudRequestTimeout );
}

void CloudDevicesService::handleControllerAbstractsListTimeout() {
    Log::warn( cc::warn( "Timeout reached waiting for cloud devices list" ) );
    m_pendingControllerAbstractsList = false;
    createPostponedTask( [this]( Services::PostponedTask& ) { requestControllerAbstractsList(); } )
        ->start( m_reaskOnErrorDelay, false );
}

void CloudDevicesService::handleControllerAbstractsListResponse( const JsonObject& response ) {
    if( hasError( response ) ) {
        Log::error( cc::error( "Error getting cloud devices list: " ) + cc::j( response[ "error" ] ) );
        m_pendingControllerAbstractsList = false;
        createPostponedTask( [this]( Services::PostponedTask& ) { requestControllerAbstractsList(); } )
            ->start( m_reaskOnErrorDelay, false );
        return;
    }

    using namespace Resources;
    int newVersion = json_get_int( response, API::DevicesList::Version, -1 );
    int notModified = json_get_int( response, API::DevicesList::NotModified, 0 );
    int currentVersion = getExistingDevicesDataVersion();

    if( currentVersion == newVersion || notModified == 1 ) {
        Log::info( cc::info( "Current cloud devices are up-to-date (v" ) +
                   cc::note( std::to_string( currentVersion ) ) + cc::info( ")" ) );
    }
    else {
        if( response.count( API::DevicesList::Devices ) == 0 ) {
            Log::error( cc::error( "Malformed cloud devices list: no devices" ) );
            m_pendingControllerAbstractsList = false;
            createPostponedTask( [this]( Services::PostponedTask& ) { requestControllerAbstractsList(); } )
                ->start( m_reaskOnErrorDelay, false );
            return;
        }

        updateDevicesList( newVersion, response[ API::DevicesList::Devices ] );

        Log::info( cc::info( "Updated cloud devices to v" ) + cc::note( std::to_string( newVersion ) ) );
    }
    m_pendingControllerAbstractsList = false;
    if( m_updateControllerAbstractsList ) {
        requestControllerAbstractsList();
    }
}

bool CloudDevicesService::checkNewVersion( const JsonObject& result ) {
    using namespace Resources;
    int newVersion = json_get_int( result, API::DeviceEvent::Version, -2 );
    if( newVersion == -2 ) {
        Log::error( cc::error( "Malformed cloud device event: no version specified" ) );
        requestControllerAbstractsList();
        return false;
    }

    int existingVersion = getExistingDevicesDataVersion();
    if( ( existingVersion != -1 ) && ( newVersion != existingVersion + 1 ) ) {
        Log::warn( cc::warn( "Looks like we've missed some cloud devices changes broadcast, received version " ) +
                   cc::num10( newVersion ) + cc::warn( ", while expecting version " ) +
                   cc::num10( existingVersion + 1 ) );
        requestControllerAbstractsList();
        return false;
    }
    return true;
}

void CloudDevicesService::handleCloudAbstractsUpdated( const JsonObject& result ) {
    using namespace Resources;

    if( !checkNewVersion( result ) ) {
        return;
    }

    const auto& devices = result[ API::DeviceEvent::Devices ];

    for( auto it = devices.begin(); it != devices.end(); ++it ) {
        if( !deviceExists( it.key() ) ) {
            Log::warn( cc::warn( "Trying to update missing cloud device" ) );
            requestControllerAbstractsList();
            return;
        }
    }

    for( auto it = devices.begin(); it != devices.end(); ++it ) {
        updateDevice( it.key(), it.value() );
    }

    setCurrentDevicesDataVersion( result[ API::DeviceEvent::Version ] );
}

void CloudDevicesService::handleCloudAbstractsAdded( const JsonObject& result ) {
    using namespace Resources;

    if( !checkNewVersion( result ) ) {
        return;
    }

    const auto& devices = result[ API::DeviceEvent::Devices ];

    for( auto it = devices.begin(); it != devices.end(); ++it ) {
        if( deviceExists( it.key() ) ) {
            Log::warn( cc::warn( "Trying to add existing cloud device" ) );
            requestControllerAbstractsList();
            return;
        }
    }

    for( auto it = devices.begin(); it != devices.end(); ++it ) {
        addDevice( it.value() );
    }

    setCurrentDevicesDataVersion( result[ API::DeviceEvent::Version ] );
}

void CloudDevicesService::handleCloudAbstractsDeleted( const JsonObject& result ) {
    using namespace Resources;

    if( !checkNewVersion( result ) ) {
        return;
    }

    JsonArray devicesIds;
    json_get_arr( result, API::DeviceEvent::Devices, devicesIds );

    for( const auto& deviceId : devicesIds ) {
        if( !deviceExists( deviceId ) ) {
            Log::warn( cc::warn( "Trying to delete missing cloud device" ) );
            requestControllerAbstractsList();
            return;
        }
    }

    for( const auto& deviceId : devicesIds ) {
        deleteDevice( deviceId );
    }

    setCurrentDevicesDataVersion( result[ API::DeviceEvent::Version ] );
}

#endif  // __WITH_CLOUD_DEVICES_LEGACY_CODE__

JsonObject CloudDevicesService::handleCloudSubscriptionCreate( const JsonObject& request ) {
    auto result = m_subscriptionManager->addSubscription( request );
    addTask( [this]() { m_subscriptionManager->sendSubscriptionList(); } );
    return result;
}

JsonObject CloudDevicesService::handleCloudSubscriptionUpdate( const JsonObject& request ) {
    auto result = m_subscriptionManager->updateSubscription( request );
    addTask( [this]() { m_subscriptionManager->sendSubscriptionList(); } );
    return result;
}

JsonObject CloudDevicesService::handleCloudSubscriptionDelete( const JsonObject& request ) {
    auto result = m_subscriptionManager->deleteSubscription( request );
    addTask( [this]() { m_subscriptionManager->sendSubscriptionList(); } );
    return result;
}

void CloudDevicesService::handleCloudRegistered() {
    Log::debug( cc::debug( "Registered on cloud" ) );
    forgetExistingAsyncRequests();
    addTask( [this]() { m_subscriptionManager->sendSubscriptionList(); } );

#if defined( __WITH_CLOUD_DEVICES_LEGACY_CODE__ )
    emmitAllDevicesReachableStateUpdated( 1 );
    requestControllerAbstractsList();
#endif  // __WITH_CLOUD_DEVICES_LEGACY_CODE__
}

void CloudDevicesService::handleCloudUnregistered() {
    Log::trace( cc::trace( "Unregistered on cloud" ) );
    forgetExistingAsyncRequests();
#if defined( __WITH_CLOUD_DEVICES_LEGACY_CODE__ )
    emmitAllDevicesReachableStateUpdated( 0 );
#endif  // __WITH_CLOUD_DEVICES_LEGACY_CODE__
}

void CloudDevicesService::handleGatewayCommandRequest( const JsonObject& params ) {
    namespace Commands = Resources::GatewayScripts::Commands;
    const std::string& command = json_get_str( params, Commands::CommandNameField );
    const Json& commandParams = params[ Commands::CommandParamsField ];
    LOGGER_TRACE_RAW( cc::trace( "Gateway command: " ) + cc::note( command ) +
                      ", command params: " + cc::j( commandParams ) );
    const Json& gatewayParams = params[ Commands::CommandParamsField ];

    if( command == Commands::StoreSubscriptionRecord ) {
        m_subscriptionManager->storeSubscriptionRecord( gatewayParams );
    }
    else if( command == Commands::RemoveSubscriptionRecord ) {
        m_subscriptionManager->removeSubscriptionRecord( gatewayParams );
    }
    else {
        LOGGER_WARN_RAW( "Gateway command not recognized: " + cc::note( command ) );
    }
}

#if defined( __WITH_CLOUD_DEVICES_LEGACY_CODE__ )

void CloudDevicesService::handleSendCommandRequest( const JsonObject& params ) {
    using namespace Resources;

    const std::string& commandId = json_get_str( params, CloudRPCParams::CommandId );
    const std::string& deviceId = json_get_str( params, CloudRPCParams::DeviceId );
    const std::string& capability = json_get_str( params, CloudRPCParams::Capability );
    const std::string& command = json_get_str( params, CloudRPCParams::Command );
    const Json& commandParams = params[ CloudRPCParams::Params ];

    auto device = getDevice( deviceId );
    if( device.empty() ) {
        Log::error( cc::error( "Trying to send command " ) + cc::note( command ) + cc::error( " to unknown device: " ) +
                    cc::note( deviceId ) );
        return;
    }

    const JsonArray& capabilities = device[ Model::Device::Capabilities ];
    if( std::find( capabilities.begin(), capabilities.end(), capability ) == capabilities.end() ) {
        Log::error( cc::error( "Trying to send command to unknown capability: " ) + cc::note( capability ) );
        return;
    }

    Json setParams;
    setParams[ API::SendCommand::Uuid ] = deviceId;
    setParams[ API::SendCommand::Capability ] = capability;
    setParams[ API::SendCommand::Command ] = command;
    setParams[ API::SendCommand::Parameters ] = commandParams;

    Json message;
    message[ API::General::Api ] = CurrentApiVersion;
    message[ API::General::Method ] = "cloud.controller_abstracts_set";
    message[ API::General::Params ] = setParams;
    sendAsyncRequest(
        GroupsNames::UI_GROUP,
        message,
        CLOUD_MESSAGE,
        std::bind( &CloudDevicesService::handleSendCommandResponse, this, commandId, std::placeholders::_1 ),
        std::bind( &CloudDevicesService::handleSendCommandTimeout, this, commandId ),
        m_cloudRequestTimeout );
}
#endif

#if defined( __WITH_CLOUD_DEVICES_LEGACY_CODE__ )

void CloudDevicesService::handleSendCommandTimeout( const std::string& commandId ) {
    Log::error( cc::error( "Timeout reached waiting for command " ) + cc::note( commandId ) + cc::error( " result" ) );
    addTask( [this, commandId]() {
        emit( CommandResponse( commandId,
                               JSON_EMPTY_OBJ,
                               JSON( "code" << -1 << "data"
                                            << "cloud_devices.error.request_timeout"
                                            << "description"
                                            << "" ) ) );
    } );
}

void CloudDevicesService::handleSendCommandResponse( const std::string& commandId, const JsonObject& response ) {
    if( hasError( response ) ) {
        Log::error( cc::error( "Error executing command " ) + cc::note( commandId ) + cc::error( ": " ) +
                    cc::j( response[ "error" ] ) );
        addTask( [this, commandId, response]() {
            emit( CommandResponse( commandId, JSON_EMPTY_OBJ, response[ "error" ] ) );
        } );
        return;
    }

    addTask( [this, commandId, response]() { emit( CommandResponse( commandId, response, JSON_EMPTY_OBJ ) ); } );
}

Json CloudDevicesService::getDeviceDiff( const std::string& deviceId, const Json& newDeviceJson ) const {
    using namespace Resources;
    auto currentDevice = getDBClient().findOne( DB::TableNames::Devices, JSON( DB::Fields::Id << deviceId ), nullptr );

    auto newDevice = newDeviceJson;
    newDevice[ DB::Fields::Id ] = deviceId;
    json_remove( newDevice, API::Common::Id );

    return jsonDiff( currentDevice, newDevice, true );
}

void CloudDevicesService::updateDevicesList( int newVersion, const Json& newDevices ) {
    using namespace Resources::DB;
    std::set< std::string > currentDevicesIds;
    auto fieldsToReturn = JSON( Fields::Id << 1 );
    auto currentDevices = getDBClient().findAll( TableNames::Devices, JSON_EMPTY_OBJ, &fieldsToReturn );
    for( auto& deviceJson : currentDevices ) {
        auto id = json_get_str( deviceJson, Fields::Id );
        if( !id.empty() ) {
            currentDevicesIds.insert( std::move( id ) );
        }
    }

    std::set< std::string > newDevicesIds;
    for( auto it = newDevices.begin(); it != newDevices.end(); ++it ) {
        newDevicesIds.insert( it.key() );
    }

    for( const auto& deviceId : currentDevicesIds ) {
        if( newDevicesIds.count( deviceId ) == 0 ) {
            deleteDevice( deviceId );
        }
    }

    for( const auto& deviceId : newDevicesIds ) {
        if( currentDevicesIds.count( deviceId ) == 0 ) {
            addDevice( newDevices[ deviceId ] );
        }
        else {
            Json deviceDiff = getDeviceDiff( deviceId, newDevices[ deviceId ] );
            updateDevice( deviceId, deviceDiff );
        }
    }

    setCurrentDevicesDataVersion( newVersion );
}

bool CloudDevicesService::deviceExists( const std::string& deviceId ) const {
    using namespace Resources;
    return getDBClient().has( DB::TableNames::Devices, JSON( DB::Fields::Id << deviceId ) );
}

Json CloudDevicesService::getDevice( const std::string& deviceId ) const {
    using namespace Resources::DB;
    return getDBClient().findOne( TableNames::Devices, JSON( Fields::Id << deviceId ), nullptr );
}

void CloudDevicesService::emmitAllDevicesReachableStateUpdated( int newState ) {
    auto fieldsToReturn = JSON( Resources::DB::Fields::Id << 1 << Resources::Model::Device::Reachable << 1 );
    auto currentDevices = getDBClient().findAll( Resources::DB::TableNames::Devices, JSON_EMPTY_OBJ, &fieldsToReturn );
    for( auto& deviceJson : currentDevices ) {
        auto deviceId = json_get_str( deviceJson, Resources::DB::Fields::Id );
        auto reachable = json_get_int( deviceJson, Resources::Model::Device::Reachable, 0 );
        if( !deviceId.empty() && reachable == 1 ) {
            auto deviceDiff = JSON( Resources::Model::Device::Reachable << newState );
            addTask( [this, deviceId, deviceDiff]() { emit( DeviceUpdated( deviceId, deviceDiff ) ); } );
        }
    }
}

void CloudDevicesService::addDevice( const JsonObject& deviceJson ) {
    using namespace Resources;
    auto device = deviceJson;
    std::string deviceId = json_get_str( device, API::Common::Id );
    if( deviceId.empty() ) {
        Log::error( cc::error( "Device object contains no id" ) );
        return;
    }
    device[ DB::Fields::Id ] = deviceId;
    json_remove( device, API::Common::Id );
    Log::debug( cc::debug( "Adding device with id " ) + cc::note( deviceId ) );
    getDBClient().insert( DB::TableNames::Devices, device );

    addTask( [this, deviceId]() { emit( DeviceAdded( deviceId ) ); } );
}

void CloudDevicesService::updateDevice( const std::string& deviceId, const JsonObject& deviceJson ) {
    using namespace Resources;
    auto device = deviceJson;
    if( device.count( API::Common::Id ) != 0 ) {
        device[ DB::Fields::Id ] = device[ API::Common::Id ];
        json_remove( device, API::Common::Id );
    }
    Log::debug( cc::debug( "Updating device with id " ) + cc::note( deviceId ) );
    getDBClient().update( DB::TableNames::Devices, JSON( DB::Fields::Id << deviceId ), JSON( "$set" << device ) );

    addTask( [this, deviceId, deviceJson]() { emit( DeviceUpdated( deviceId, deviceJson ) ); } );
}

void CloudDevicesService::deleteDevice( const std::string& deviceId ) {
    using namespace Resources;
    Log::debug( cc::debug( "Removing device with id " ) + cc::note( deviceId ) );
    getDBClient().remove( DB::TableNames::Devices, JSON( DB::Fields::Id << deviceId ) );

    addTask( [this, deviceId]() { emit( DeviceRemoved( deviceId ) ); } );
}

#endif  // __WITH_CLOUD_DEVICES_LEGACY_CODE__

void CloudDevicesService::forgetExistingAsyncRequests() {
    discardExistingAsyncRequestResponses();
    m_pendingControllerAbstractsList = false;
    m_updateControllerAbstractsList = false;
}

}  // namespace CloudDevices
}  // namespace HubPlatform
