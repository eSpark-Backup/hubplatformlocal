#include "cloud_storage/cloud_storage_impl.h"
#include "cloud_storage/resources.h"
#include "database_client/idbclient.h"
#include "log_utils/ezlo_log.h"
#include "json_utils/json_utils.h"
#include "provision_data_utils/provision_data_manager.h"
#include "system_utils/filesystem.h"
#include "http/http_events.h"
#include "http/resources.h"

namespace HubPlatform {
namespace Common {
namespace CloudStorageInterface {

CloudStorageImpl::CloudStorageImpl( IDbClient& dbClient )
: m_dbClient( dbClient ) {
    setHttpReceivingMethod();
    setHttpClosingConnectionMethod();
}

CloudStorageImpl::~CloudStorageImpl() {
    if( m_http.joinable() ) {
        m_http.join();
        Log::trace( cc::trace( "CloudStorageImpl: Exit from join in plugin " ) );
    }
}

Errors CloudStorageImpl::uploadFile( const UploadFileData& data, StorageType storageType, UploadResponse response ) {
    return uploadFile( LoadType::Upload, data, storageType, response );
}

Errors CloudStorageImpl::uploadLogs( const UploadFileData& data,
                                     StorageType storageType,
                                     CloudStorageImpl::UploadResponse response ) {
    return uploadFile( LoadType::UploadLogs, data, storageType, response );
}

Errors CloudStorageImpl::uploadFile( LoadType loadType,
                                     const UploadFileData& data,
                                     StorageType storageType,
                                     CloudStorageImpl::UploadResponse response ) {
    using namespace Resources::Http::Options;
    CommonHttp::HttpOptions httpOptions;
    filesystem::File::Name filename;
    filesystem::File::Extension extension;
    try {
        std::tie( filename, extension ) = filesystem::getFilenameAndExtension( data.file );
    }
    catch( const std::exception& ex ) {
        LOGGER_ERROR( ex.what() );
    }
    httpOptions.type = "POST";
    httpOptions.skipSecurity = false;
    httpOptions.handler = "empty";
    httpOptions.headers.insert( { Headers::Type, toString( storageType ) } );
    httpOptions.headers.insert( { Headers::Filename, std::move( filename ) } );
    httpOptions.headers.insert( { Headers::FileFormat, std::move( extension ) } );
    if( !data.metadata.empty() ) {
        httpOptions.headers.insert( { Headers::Meta, data.metadata.dump() } );
    }
    if( !data.uuid.empty() ) {
        httpOptions.headers.insert( { Headers::Uuid, data.uuid } );
    }
    httpOptions.contentType = "";
    httpOptions.mimeParts.push_back( { "file", data.file, CommonHttp::Mime::DataType::FILE } );
    httpOptions.timeoutMs = HttpResponseTimeoutMs;
    httpOptions.connectTimeoutMs = HttpConnectionTimeoutMs;
    httpOptions.redirection = 0;

    Errors error;
    CommonHttp::Http::RequestId requestId;
    std::tie( error, requestId ) = sendHttpRequest( loadType, httpOptions );
    if( error == Errors::Ok ) {
        std::lock_guard< std::mutex > lock( m_mutex );
        m_uploadResponses.insert( { requestId, std::move( response ) } );
    }
    return error;
}

Errors CloudStorageImpl::downloadFile( const FileKey& fileKey, const Path& downloadPath, DownloadResponse response ) {
    using namespace Resources::Http::Options;
    CommonHttp::HttpOptions httpOptions;
    httpOptions.type = "POST";
    httpOptions.skipSecurity = false;
    httpOptions.contentType = "application/json";
    httpOptions.handler = "empty";
    httpOptions.headers.insert( { Headers::TransferEncoding, "chunked" } );
    httpOptions.data = fileKey.toJson().dump();
    httpOptions.receivedDataFilePath = downloadPath;
    httpOptions.timeoutMs = HttpResponseTimeoutMs;
    httpOptions.connectTimeoutMs = HttpConnectionTimeoutMs;

    Errors error;
    CommonHttp::Http::RequestId requestId;
    std::tie( error, requestId ) = sendHttpRequest( LoadType::Download, httpOptions );
    if( error == Errors::Ok ) {
        std::lock_guard< std::mutex > lock( m_mutex );
        m_downloadsInfo.insert( { requestId, { std::move( response ), "" } } );
    }
    return error;
}

void CloudStorageImpl::stopTransfer() {
    m_http.stop();
}

std::pair< Errors, CommonHttp::Http::RequestId > CloudStorageImpl::sendHttpRequest(
    LoadType loadType,
    CommonHttp::HttpOptions& httpOptions ) {
    try {
        using namespace Resources;
        auto data = m_dbClient.findOne( TempCloudStorageDBName, JSON_EMPTY_OBJ, nullptr );
        auto& provision = ProvisionDataUtils::getManager().getProvisionData();
        auto token = provision.decryptSensitiveData( json_get_str( data, DB::Fields::Token ) );
        if( token.empty() ) {
            return { Errors::InvalidToken, {} };
        }
        if( loadType == LoadType::UploadLogs ) {
            auto urls = data[ DB::Fields::UploadLogsUrls ];
            if( urls.empty() ) {
                urls = data[ DB::Fields::UploadUrls ];
                if( urls.empty() ) {
                    return { Errors::InvalidUrl, {} };
                }
            }
            auto xapp = json_get_str( data, DB::Fields::Xapp );
            if( xapp.empty() ) {
                return { Errors::InvalidXapp, {} };
            }
            httpOptions.url = json_get_str( urls[ 0 ] );
            httpOptions.headers.insert( { Http::Options::Headers::Xapp, xapp } );
        }
        else if( loadType == LoadType::Upload ) {
            auto& urls = data[ DB::Fields::UploadUrls ];
            if( urls.empty() ) {
                return { Errors::InvalidUrl, {} };
            }
            auto xapp = json_get_str( data, DB::Fields::Xapp );
            if( xapp.empty() ) {
                return { Errors::InvalidXapp, {} };
            }
            httpOptions.url = json_get_str( urls[ 0 ] );
            httpOptions.headers.insert( { Http::Options::Headers::Xapp, xapp } );
        }
        else {
            auto& urls = data[ DB::Fields::DownloadUrls ];
            if( urls.empty() ) {
                return { Errors::InvalidUrl, {} };
            }
            httpOptions.url = json_get_str( urls[ 0 ] );
        }
        std::string logMsg = "Headers: \n";
        for( const auto& header : httpOptions.headers ) {
            logMsg += "        " + header.first + ": " + header.second + "\n";
        }
        logMsg += "    Authorization: " + token.substr( 0, token.find_last_of( '.' ) ) + "\n";
        logMsg += "    url: " + httpOptions.url + "\n";
        LOGGER_DEBUG( logMsg );
        httpOptions.headers.insert( { "Authorization", "Bearer " + token } );
        auto requestId = m_http.request( "cloudStorage", httpOptions );
        return { Errors::Ok, requestId };
    }
    catch( const std::exception& ex ) {
        LOGGER_ERROR( ex.what() );
        return { Errors::RequestError, {} };
    }
}

void CloudStorageImpl::setHttpReceivingMethod() {
    using namespace CommonHttp::Resources;
    auto eventName = HttpEvents::HttpDataReceived;
    m_http.on( eventName, [this]( const std::string& /*eventHandler*/, const CommonHttp::EventPtr& event ) {
        if( !event.get() )
            return;
        std::lock_guard< std::mutex > lock( m_mutex );
        auto requestId = event->id();
        auto uploadResponseIt = m_uploadResponses.find( requestId );
        if( uploadResponseIt != m_uploadResponses.end() ) {
            auto response = uploadResponseIt->second;
            m_uploadResponses.erase( uploadResponseIt );
            sendResponse( event->toJson(), response, "http_data_received" );
            return;
        }
        auto downloadInfoIt = m_downloadsInfo.find( requestId );
        if( downloadInfoIt != m_downloadsInfo.end() ) {
            using namespace Resources::Http::Response;
            auto info = downloadInfoIt->second;
            auto message = event->toJson();
            auto code = json_get_int( message, Code );
            if( info.error.empty() && code != 200 ) {
                json_get( message, std::vector< Json >{ Reason, Text }, info.error );
            }
            return;
        }

        LOGGER_ERROR( "Response not found" );
    } );
}

void CloudStorageImpl::setHttpClosingConnectionMethod() {
    using namespace CommonHttp::Resources;
    m_http.on( HttpEvents::HttpConnectionClosed,
               [this]( const std::string& /*eventHandler*/, const CommonHttp::EventPtr& event ) {
                   if( !event.get() )
                       return;
                   std::lock_guard< std::mutex > lock( m_mutex );
                   auto requestId = event->id();
                   auto uploadResponseIt = m_uploadResponses.find( requestId );
                   if( uploadResponseIt != m_uploadResponses.end() ) {
                       auto response = uploadResponseIt->second;
                       m_uploadResponses.erase( uploadResponseIt );
                       sendResponse( event->toJson(), response, "http_connection_closed" );
                       return;
                   }
                   auto downloadInfoIt = m_downloadsInfo.find( requestId );
                   if( downloadInfoIt != m_downloadsInfo.end() ) {
                       using namespace Resources::Http::Response;
                       auto info = downloadInfoIt->second;
                       auto message = event->toJson();
                       int reasonCode;
                       json_get( message, std::vector< Json >{ Reason, Code }, reasonCode );
                       if( info.error.empty() &&
                           static_cast< CommonHttp::Reason >( reasonCode ) != CommonHttp::Reason::NONE ) {
                           json_get( message, std::vector< Json >{ Reason, Text }, info.error );
                       }
                       info.response( info.error );
                       m_downloadsInfo.erase( downloadInfoIt );
                       return;
                   }
               } );
}

void CloudStorageImpl::sendResponse( const Json& message,
                                     CloudStorageImpl::UploadResponse callback,
                                     const std::string& errorMessage ) {
    try {
        using namespace Resources::Http::Response;
        std::string error;
        FileKey fileKey;
        if( json_get_int( message, Code ) == 200 ) {
            auto data = Json::parse( json_get_str( message, Data ) );
            fileKey.key = json_get_str( data[ "file" ], Key );
            fileKey.uuid = json_get_str( data[ "file" ], Uuid );
        }
        else {
            json_get( message, std::vector< Json >{ Reason, Text }, error );
        }
        callback( error, fileKey );
    }
    catch( const std::exception& ex ) {
        LOGGER_ERROR( errorMessage + ": " + ex.what() );
    }
}

}  // namespace CloudStorageInterface
}  // namespace Common
}  // namespace HubPlatform
