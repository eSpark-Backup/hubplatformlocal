#pragma once

#include "http_bridge/endpoint.h"
#include "http_bridge/endpoint_handlers.h"
#include "system_utils/filesystem.h"
#include <memory>
#include <mutex>

namespace HubPlatform {
namespace HttpBridge {

class ApiBridge;

class ICamerasEndpointHandler {
public:
    using File = std::shared_ptr< FILE >;

    ~ICamerasEndpointHandler() = default;

    virtual void getSnapshot( CallbackParams params, const std::string& streamId ) = 0;
    virtual size_t getFileSize( CallbackParams params );
    virtual size_t readFile( CallbackParams params, char* dst, size_t size );
    virtual void removeFile( CallbackParams params );
    virtual bool isHandlerValid() = 0;
    virtual std::string getErrorMessage( CallbackParams params );
    virtual void setErrorMessage( CallbackParams params, const std::string& errorMessage );

protected:
    void callbackOnWritable( CallbackParams params );

    struct FileAutoRemover {
        ~FileAutoRemover() {
            filesystem::removeFile( filePath.c_str() );
        }
        std::string filePath;
    };

    struct FileState {
        File file;
        std::string error;
        size_t fileSize;
        FileAutoRemover fileAutoRemover;
    };

    std::mutex m_mutex;
    std::map< void*, FileState > m_files;
};

class EmptyCamerasEndpointHandler : public ICamerasEndpointHandler {
public:
    void getSnapshot( CallbackParams params, const std::string& streamId ) override;
    bool isHandlerValid() override;
};

class CamerasAddonEndpoint : public Endpoint, public EndpointHandlers< CamerasAddonEndpoint > {
public:
    explicit CamerasAddonEndpoint( ApiBridge& apiBridge );
    static EndpointPtr makeEndpoint( ApiBridge& apiBridge );

public:
    ~CamerasAddonEndpoint() override = default;
    int endpointCallback( struct lws* wsi,
                          enum lws_callback_reasons reason,
                          void* user,
                          void* in,
                          size_t len ) override;

    void setEndpointHandler( std::unique_ptr< ICamerasEndpointHandler >&& handler );

protected:
    virtual int httpDummy( CallbackParams params );
    virtual int onHttp( CallbackParams params, BufferPosition bufPos );
    virtual int onHttpWriteable( CallbackParams params, BufferPosition bufPos );
    virtual int onClosedHttp( CallbackParams params, BufferPosition bufPos );

private:
    std::unique_ptr< ICamerasEndpointHandler > m_endpointHandler;
    ApiBridge& m_api;

    static const int ConnectionTimeout = 70;
};

}  // namespace HttpBridge
}  // namespace HubPlatform
