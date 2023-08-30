#include "cloud_storage/cloud_storage_utils.h"
#include "json_utils/json_utils.h"

namespace HubPlatform {
namespace Common {
namespace CloudStorageInterface {

void getFilesList( Services::Service& service,
                   StorageType storageType,
                   Services::Service::ReceiveCallback callback,
                   const std::chrono::seconds& timeout ) {
    Json params;
    params[ "type" ] = toString( storageType );
    JsonObject message;
    message[ "method" ] = "cloud.storage_list";
    message[ "api" ] = "1";
    message[ "params" ] = std::move( params );

    service.sendAsyncRequest( GroupsNames::UI_GROUP, message, CLOUD_MESSAGE, std::move( callback ), timeout );
}

void prepareUpload( Services::Service& service,
                    PrepareUploadType prepareUploadType,
                    Services::Service::ReceiveCallback callback,
                    std::chrono::seconds requestTimeout ) {
    Json params;
    params[ "type" ] = toString( prepareUploadType );
    JsonObject message;
    message[ "method" ] = "cloud.storage_prepare_upload";
    message[ "api" ] = "1";
    message[ "params" ] = std::move( params );
    LOGGER_DEBUG_RAW( cc::debug( "cloud.storage_prepare_upload: using 'params' parameter instead of 'data'" ) );

    service.sendAsyncRequest( GroupsNames::UI_GROUP, message, CLOUD_MESSAGE, std::move( callback ), requestTimeout );
}

}  // namespace CloudStorageInterface
}  // namespace Common
}  // namespace HubPlatform
