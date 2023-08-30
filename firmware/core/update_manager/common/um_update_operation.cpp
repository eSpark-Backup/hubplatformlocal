#include "update_manager/common/um_update_operation.h"
#include "update_manager/resources/um_resources.h"

#include "json_utils/json_utils.h"

#include <cstring>

namespace HubPlatform {
namespace UpdateManager {

UpdateOperation::UpdateOperation()
: UpdateOperation( Opkg::PackageSummary{}, UpdateOperationType::Unknown, "", 0 ) {}

UpdateOperation::UpdateOperation( const Opkg::PackageSummary& packageSummary,
                                  UpdateOperationType operationType,
                                  const std::string& source,
                                  int snapshot )
: packageSummary( packageSummary )
, operationType( operationType )
, componentType()
, source( source )
, snapshot( snapshot )
, componentHash() {}

// json = {
//        "operation": "upgrade",
//        "source": "http://dl.mios.com/linux_firmware/qa",
//        "snapshot": 42,
//        "type": "plugin",
//        "id": "zwave",
//        "version": "1.3.42"
//    }

UpdateOperation UpdateOperation::fromJson( const JsonObject& json ) {
    namespace Fields = Resources::UpdateOperationFields;
    UpdateOperation operation;

    operation.componentType = updateComponentTypeFromString( json_get_str( json, Fields::Type ) );

    if( operation.componentType == UpdateComponentType::Firmware ) {
        operation.packageSummary.name = Resources::PackageNames::Firmware;
    }
    else if( operation.componentType == UpdateComponentType::Addon ) {
        auto id = json_get_str( json, Fields::Id );
        operation.packageSummary.name = Resources::PackageNames::AddonPrefix + id;
    }
    else if( operation.componentType == UpdateComponentType::Plugin ) {
        auto id = json_get_str( json, Fields::Id );
        operation.packageSummary.name = Resources::PackageNames::PluginPrefix + id;
    }
    else if( operation.componentType == UpdateComponentType::CustomPlugin ) {
        operation.packageSummary.name = json_get_str( json, Fields::Id );
        operation.componentHash = json_get_str( json, Fields::ArchiveHashMd5 );

        if( operation.packageSummary.name.empty() ) {
            return {};
        }
    }
    else {
        return {};
    }

    namespace Fields = Resources::UpdateOperationFields;
    operation.operationType = updateOperationTypeFromString( json_get_str( json, Fields::Operation ) );
    if( operation.operationType == UpdateOperationType::Unknown ) {
        return {};
    }

    if( operation.componentType == UpdateComponentType::CustomPlugin &&
        ( operation.operationType == UpdateOperationType::Install ||
          operation.operationType == UpdateOperationType::Upgrade ) ) {
        if( operation.componentHash.empty() ) {
            return {};
        }
    }

    operation.source = json_get_str( json, Fields::Source );
    operation.snapshot = json_get_int( json, Fields::Snapshot, 0 );
    auto versionString = json_get_str( json, Fields::Version );
    if( !versionString.empty() )
        operation.packageSummary.version = versionString;

    return operation;
}

JsonObject UpdateOperation::toJson() const {
    namespace Fields = Resources::UpdateOperationFields;
    JsonObject json = JSON_EMPTY_OBJ;
    json[ Fields::Operation ] = updateOperationTypeToString( operationType );
    json[ Fields::Source ] = source;
    json[ Fields::Snapshot ] = snapshot;

    json[ Fields::Type ] = updateComponentTypeToString( componentType );

    if( packageSummary.name == Resources::PackageNames::Firmware ) {
        json[ Fields::Type ] = updateComponentTypeToString( UpdateComponentType::Firmware );
    }
    else if( strncmp( packageSummary.name.c_str(),
                      Resources::PackageNames::AddonPrefix,
                      strlen( Resources::PackageNames::AddonPrefix ) ) == 0 ) {
        json[ Fields::Type ] = updateComponentTypeToString( UpdateComponentType::Addon );
        json[ Fields::Id ] = packageSummary.name.substr( strlen( Resources::PackageNames::AddonPrefix ) );
    }
    else if( strncmp( packageSummary.name.c_str(),
                      Resources::PackageNames::PluginPrefix,
                      strlen( Resources::PackageNames::PluginPrefix ) ) == 0 ) {
        json[ Fields::Type ] = updateComponentTypeToString( UpdateComponentType::Plugin );
        json[ Fields::Id ] = packageSummary.name.substr( strlen( Resources::PackageNames::PluginPrefix ) );
    }

    json[ Fields::Version ] = packageSummary.version;

    if( componentType == UpdateComponentType::CustomPlugin ) {
        json[ Fields::ArchiveHashMd5 ] = componentHash;
        json[ Fields::Id ] = packageSummary.name;
    }

    return json;
}

bool UpdateOperation::isValid() const {
    return operationType != UpdateOperationType::Unknown;
}

}  // namespace UpdateManager
}  // namespace HubPlatform
