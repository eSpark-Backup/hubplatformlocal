#include "cloud_storage/cloud_storage_interface.h"
#include "cloud_storage/cloud_storage_impl.h"
#include "database_client/idbclient.h"
#include "common_utils/cpp14.h"

namespace HubPlatform {
namespace Common {
namespace CloudStorageInterface {

RemoteInterface::RemoteInterface( IDbClient& dbClient )
: m_pimpl( new CloudStorageImpl( dbClient ) ) {}

RemoteInterface::~RemoteInterface() {
    delete m_pimpl;
}

Errors RemoteInterface::uploadFile( const UploadFileData& data, StorageType storageType, UploadResponse response ) {
    return m_pimpl->uploadFile( data, storageType, std::move( response ) );
}

Errors RemoteInterface::uploadLogs( const UploadFileData& data,
                                    StorageType storageType,
                                    RemoteInterface::UploadResponse response ) {
    return m_pimpl->uploadLogs( data, storageType, std::move( response ) );
}

Errors RemoteInterface::downloadFile( const FileKey& fileKey, const Path& downloadPath, DownloadResponse response ) {
    return m_pimpl->downloadFile( fileKey, downloadPath, std::move( response ) );
}

void RemoteInterface::stopTransfer() {
    m_pimpl->stopTransfer();
}

}  // namespace CloudStorageInterface
}  // namespace Common
}  // namespace HubPlatform
