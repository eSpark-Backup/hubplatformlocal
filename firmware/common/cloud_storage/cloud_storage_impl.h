#pragma once

#include "cloud_storage/cloud_storage_type.h"
#include "json_utils/json_fwd.h"
#include "http/http.h"

#include <string>
#include <chrono>

class IDbClient;

namespace HubPlatform {

namespace CommonHttp {
struct HttpOptions;
}  // namespace CommonHttp

namespace Common {
namespace CloudStorageInterface {

class CloudStorageImpl final {
public:
    using UploadResponse = std::function< void( const std::string& error, const FileKey& fileKey ) >;
    using DownloadResponse = std::function< void( const std::string& error ) >;
    using Path = std::string;
    using RequestId = CommonHttp::Http::RequestId;
    using UploadResponses = std::map< RequestId, UploadResponse >;

    explicit CloudStorageImpl( IDbClient& dbClient );
    ~CloudStorageImpl();

    /* @brief return file key as response. In case of error returns empty key */
    Errors uploadFile( const UploadFileData& data, StorageType storageType, UploadResponse response );
    Errors uploadLogs( const UploadFileData& data, StorageType storageType, UploadResponse response );
    Errors downloadFile( const FileKey& fileKey, const Path& downloadPath, DownloadResponse response );
    void stopTransfer();

private:
    const std::chrono::seconds m_requestTimeout = std::chrono::seconds( 90 );
    const int HttpResponseTimeoutMs = 5 * 60 * 1000;
    const int HttpConnectionTimeoutMs = 30 * 1000;

private:
    Errors uploadFile( LoadType loadType,
                       const UploadFileData& data,
                       StorageType storageType,
                       UploadResponse response );

    std::pair< Errors, CommonHttp::Http::RequestId > sendHttpRequest( LoadType loadType,
                                                                      CommonHttp::HttpOptions& httpOptions );
    void setHttpReceivingMethod();
    void setHttpClosingConnectionMethod();

    void sendResponse( const Json& message, UploadResponse callback, const std::string& errorMessage );

private:
    std::mutex m_mutex;
    IDbClient& m_dbClient;
    CommonHttp::Http m_http;
    UploadResponses m_uploadResponses;

    struct DownloadInfo {
        DownloadResponse response;
        std::string error;
    };

    std::map< RequestId, DownloadInfo > m_downloadsInfo;
};

}  // namespace CloudStorageInterface
}  // namespace Common
}  // namespace HubPlatform
