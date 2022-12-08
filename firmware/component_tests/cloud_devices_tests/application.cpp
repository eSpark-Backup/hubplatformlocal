#include "cloud_devices_tests/application.h"
#include "test_utils/test_environment.h"
#include "update_manager/resources/um_resources.h"

#include "json_utils/json_utils.h"
#include "jsonrpc/message.h"

namespace HubPlatform {
namespace Tests {

CloudDevicesTestsApplication::CloudDevicesTestsApplication()
: ComponentTestApplication( Tests::getComponentTestsFolder() + "/cloud_devices_tests/root" ) {}

void CloudDevicesTestsApplication::beforeApplicationsStartup() {
    ComponentTestApplication::beforeApplicationsStartup();

    join( GroupsNames::EXTERNAL_DEVICE_GROUP );

    setRequestsProcessFunction( [this]( rpc::Message& message ) {
        std::unique_lock< std::mutex > lock( m_requestsMutex );
        m_requestsMessages.push_back( message.getMessage() );
        lock.unlock();
        m_requestsCondVar.notify_all();
    } );
}

void CloudDevicesTestsApplication::setUpAdditionalApplications() {
    createLinkToAddonInTestFolder( "ha-luad" );

    executeCoreService( "ha-extensions_manager", "extensions_manager" );
    executeCoreService( "ha-external_controllers", "external_controllers" );
    executeCoreService( "ha-cloud_devices", "cloud_devices" );

    executeAddonService( "ha-luad", "lua" );

    // Need this for extensions manager if we do not run update_manager service
    getDBClient().update( UpdateManager::Resources::DBTableNames::UnavailablePlugins,
                          JSON_EMPTY_OBJ,
                          JSON( "_id"
                                << "UM.initialized" ),
                          true,
                          false );
}

void CloudDevicesTestsApplication::afterApplicationsStartup() {
    std::unique_lock< std::mutex > lock( m_requestsMutex );
    m_requestsCondVar.wait_for( lock, std::chrono::seconds( 30 ), [this] { return !m_requestsMessages.empty(); } );
    auto it = std::find_if( m_requestsMessages.rbegin(), m_requestsMessages.rend(), []( const JsonObject& value ) {
        return json_get_str( value, "method" ) == "cloud.controller_abstracts_list";
    } );
    if( it == m_requestsMessages.rend() ) {
        throw ComponentTestApplicationError( "Expected cloud.controller_abstracts_list request!" );
    }
    auto& request = *it;
    sendResponseToHub( request, parseJson( R"({ "version": -1, "not_modified": 1 })" ) );

    resetRequestsProcessFunction();

    ComponentTestApplication::afterApplicationsStartup();
}

bool CloudDevicesTestsApplication::shouldUseRandomLocalPort() {
    return true;
}

}  // namespace Tests
}  // namespace HubPlatform
