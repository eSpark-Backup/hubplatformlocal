#include "cored/features_manager.h"
#include "cored/model/resources/cored_resources.h"
#include "scenes_manager/scenes_engine/advanced_scenes_version.h"

#include "base_utils/semantic_version.h"
#include "database_client/dbqueryparser.h"
#include "json_utils/json_utils.h"

#include <vector>

namespace HubPlatform {
namespace CoreD {

#if defined( __HUB_WITH_WEBRTC__ )
#define WITH_WEBRTC true
#else
#define WITH_WEBRTC false
#endif

IFeaturesManager::~IFeaturesManager() = default;

FeaturesManager::FeaturesManager( IDbClient& dbClient )
: m_dbClient( dbClient ) {
    using namespace Model::Resources;

    struct ControllerFeature {
        std::string name;
        Common::SemanticVersion< int > version;
        bool status;
        bool editable;
    };

    static std::vector< ControllerFeature > ControllerFeatures = {
        { "local_mode", { 1, 0 }, true, false },
        { "advanced_scenes",
          { FEATURE_ADVANCED_SCENES_VERSION_MAJOR, FEATURE_ADVANCED_SCENES_VERSION_MINOR },
          true,
          false },
        { "cloud_devices", { 1, 0 }, true, false },
        { "cameras", { 1, 0 }, true, false },
        { "device_settings", { 1, 0 }, true, false },
        { "backups", { 1, 0 }, false, false },
        { "webrtc", { 1, 0 }, WITH_WEBRTC, true },
#if defined( __HUB_TYPE_H2_SECURE__ ) || defined( __HUB_TYPE_LOCAL__ )
        { "protect", { 2, 0 }, true, true },
        { "protect_delayed", { 1, 0 }, true, false },
#elif defined( __HUB_TYPE_H2__ )
        { "protect", { 2, 0 }, false, true },
        { "protect_delayed", { 1, 0 }, true, false },
#elif defined( __HUB_TYPE_VHUB__ )
    // "protect" is not available
#elif defined( __HUB_TYPE_BEEWAVE__ )
    // "protect" is not available
#elif defined( __HUB_TYPE_G150__ )
    // "protect" is not available
#else
#error "Please specify controller features for current controller type"
#endif
    };

    for( auto& feature : ControllerFeatures ) {
        auto featurePresent = m_dbClient.has( DBNames::FeaturesDB, JSON( db::ID_FIELD_NAME << feature.name ) );
        if( !featurePresent ) {
            m_dbClient.insert(
                DBNames::FeaturesDB,
                JSON( db::ID_FIELD_NAME << feature.name << "version" << feature.version.toString() << "status"
                                        << feature.status << "editable" << feature.editable ) );
        }
        else {
            m_dbClient.update(
                DBNames::FeaturesDB,
                JSON( db::ID_FIELD_NAME << feature.name ),
                JSON( "$set" << JSON( "version" << feature.version.toString() << "editable" << feature.editable ) ) );
        }
    }

    auto featuresArray = m_dbClient.findAll( DBNames::FeaturesDB, JSON_EMPTY_OBJ, nullptr );

    for( int i = 0; i < featuresArray.size(); i++ ) {
        bool isPresent = false;
        for( auto& feature : ControllerFeatures ) {
            if( featuresArray[ i ][ db::ID_FIELD_NAME ] == feature.name ) {
                isPresent = true;
                break;
            }
        }
        if( !isPresent ) {
            m_dbClient.remove( DBNames::FeaturesDB,
                               JSON( db::ID_FIELD_NAME << featuresArray[ i ][ db::ID_FIELD_NAME ] ) );
        }
    }
}

void FeaturesManager::setFeatureStatus( const std::string& feature, bool status ) {
    using namespace Model::Resources;
    m_dbClient.update(
        DBNames::FeaturesDB, JSON( db::ID_FIELD_NAME << feature ), JSON( "$set" << JSON( "status" << status ) ) );
}

bool FeaturesManager::getFeatureStatus( const std::string& feature ) const {
    using namespace Model::Resources;

    const auto featureJs = m_dbClient.findOne( DBNames::FeaturesDB, JSON( db::ID_FIELD_NAME << feature ), nullptr );
    return !featureJs.empty() && json_get_bool( featureJs, "status", false );
}

}  // namespace CoreD
}  // namespace HubPlatform
