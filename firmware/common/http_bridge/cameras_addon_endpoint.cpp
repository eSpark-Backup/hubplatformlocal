#include "http_bridge/cameras_addon_endpoint.h"
#include "http_bridge/ws_api_bridge.h"
#include "http_bridge/resources.h"

#include "base_utils/ezlodb.h"
#include "database_client/db_flags_names.h"
#include "system_utils/filesystem.h"
#include "common_utils/cpp14.h"

namespace HubPlatform {
namespace HttpBridge {

const int CamerasAddonEndpoint::ConnectionTimeout;

CamerasAddonEndpoint::CamerasAddonEndpoint( ApiBridge& apiBridge )
: EndpointHandlers( { { LWS_CALLBACK_HTTP, &CamerasAddonEndpoint::onHttp },
                      { LWS_CALLBACK_HTTP_WRITEABLE, &CamerasAddonEndpoint::onHttpWriteable },
                      { LWS_CALLBACK_CLOSED_HTTP, &CamerasAddonEndpoint::onClosedHttp } } )
, m_endpointHandler( std::make_unique< EmptyCamerasEndpointHandler >() )
, m_api( apiBridge ) {}

EndpointPtr CamerasAddonEndpoint::makeEndpoint( ApiBridge& apiBridge ) {
    std::unique_ptr< CamerasAddonEndpoint > ptr( new CamerasAddonEndpoint( apiBridge ) );
    return EndpointPtr{ ptr.release() };
}

/** Http to WS API bridge. Implements methods:
 *  - /v1/addons/cameras/stream/snapshot/<stream_id>
 */
int CamerasAddonEndpoint::endpointCallback( struct lws* wsi,
                                            enum lws_callback_reasons reason,
                                            void* user,
                                            void* in,
                                            size_t len ) {
    uint8_t buf[ LWS_PRE + LWS_RECOMMENDED_MIN_HEADER_SPACE ];
    uint8_t *start = &buf[ LWS_PRE ], *p = start, *end = &buf[ sizeof( buf ) - LWS_PRE - 1 ];
    CallbackParams params{ wsi, reason, user, in, len };
    BufferPosition bufPos{ start, p, end };

    const auto it = m_handlers.find( reason );
    if( it != m_handlers.end() ) {
        const auto handler = it->second;
        return ( this->*handler )( params, bufPos );
    }
    return httpDummy( params );
}

void CamerasAddonEndpoint::setEndpointHandler( std::unique_ptr< ICamerasEndpointHandler >&& handler ) {
    m_endpointHandler = std::move( handler );
}

int CamerasAddonEndpoint::httpDummy( CallbackParams params ) {
    return HttpBridge::httpDummy( params );
}

int CamerasAddonEndpoint::onHttp( CallbackParams params, BufferPosition bufPos ) {
    auto* wsi = params.m_wsi;
    char* url = (char*) params.m_in;
    auto *start = bufPos.m_start, *&p = bufPos.m_p, *end = bufPos.m_end;

    SessionPtr session = m_api.startSession( wsi, Resources::sessionTypeHttp );
    if( !session.get() || !session->getData() )
        return -1;
    SessionData& sessionData = *session->getData();

    if( !m_endpointHandler->isHandlerValid() ) {
        auto error = "no cameras addon";
        LOGGER_TRACE( error );
        m_endpointHandler->setErrorMessage( params, error );
        lws_callback_on_writable( params.m_wsi );
        return 0;
    }

    if( !url || std::strlen( url ) == 0 ) {
        auto error = "stream id not found";
        LOGGER_WARN( error );
        m_endpointHandler->setErrorMessage( params, error );
        lws_callback_on_writable( params.m_wsi );
        return 0;
    }

    lws_set_timeout( wsi, PENDING_TIMEOUT_HTTP_KEEPALIVE_IDLE, ConnectionTimeout );

    std::string urlString{ Resources::camerasAddonEndPoint };
    urlString += url;
    sessionData.getPath().assign( urlString );

    LOGGER_DEBUG_RAW( cc::debug( "Camera stream url:" ) + cc::u( url ) );
    session->extractHeaders( wsi );

    // if command is ok and all its data came in (for POST requests), call onMessage
    if( session->method == WSI_TOKEN_POST_URI ) {
        // wait for POST body completion, then call ApiBridge::onMessage().
    }
    else {
        getApiBridge().emit( Resources::eventAuthorization, "cameras", session );

        auto sessionCount = m_api.sessionCount();
        if( sessionCount > ApiBridge::parallelSessionsLimit() ) {
            const bool closeSession = true;
            ApiBridge::errorParallelSessionLimitReached( sessionData, sessionCount, start, p, end, closeSession );
            return 1;
        }

        // get <stream_id> here from an URL
        std::string tmpUrl = url;
        auto streamIdPos = tmpUrl.find_last_of( '/' ) + 1;
        if( streamIdPos == std::string::npos ) {
            auto error = "stream id not found";
            LOGGER_WARN( error );
            m_endpointHandler->setErrorMessage( params, error );
            lws_callback_on_writable( params.m_wsi );
            return 0;
        }
        auto streamId = tmpUrl.substr( streamIdPos );
        m_endpointHandler->getSnapshot( params, streamId );
    }

    return 0;
}

int CamerasAddonEndpoint::onHttpWriteable( CallbackParams params, BufferPosition bufPos ) {
    auto* wsi = params.m_wsi;
    auto *start = bufPos.m_start, *&p = bufPos.m_p, *end = bufPos.m_end;

    LOGGER_TRACE_PRINT( "httpCallback / http_writeable: %u, wsi: %p", params.m_len, wsi );

    int n = LWS_WRITE_HTTP;

    SessionPtr pss = m_api.findSession( wsi );
    if( !pss || !pss->getData() ) {
        return -1;
    }
    auto& sessionData = *pss->getData();
    if( !sessionData.headersSent() ) {
        // Send headers here
        if( !sessionData.isLoggedIn() && !Db::getFlag( OfflineAnonymousAccessFlagName ) ) {
            LOGGER_TRACE_RAW( cc::trace( "Wasn't authorized; wsi: " ) + cc::num016( (uint64_t) wsi ) );
            const bool closeSession = true;
            return ApiBridge::writeHttpStatusHeaders(
                sessionData, HTTP_STATUS_UNAUTHORIZED, "", start, p, end, closeSession );
        }
        else {
            LOGGER_TRACE_RAW( cc::trace( "Authorization OK; wsi: " ) + cc::num016( (uint64_t) wsi ) );
            if( !m_endpointHandler->getErrorMessage( params ).empty() ) {
                const char* contentType = "text/html";
                ApiBridge::writeHttpStatusHeaders(
                    sessionData, HTTP_STATUS_NOT_FOUND, contentType, start, p, end, false );
            }
            else {
                const char* contentType = "image/jpeg";
                ApiBridge::writeHttpStatusHeaders( sessionData, HTTP_STATUS_OK, contentType, start, p, end, false );
            }
            sessionData.headersSent( true );
            lws_callback_on_writable( wsi );
            return 0;
        }
    }
    auto error = m_endpointHandler->getErrorMessage( params );
    size_t messageSize;
    if( error.empty() ) {
        messageSize = m_endpointHandler->getFileSize( params );
    }
    else {
        messageSize = error.size();
    }
    // send body here
    /* write using portions of a size defined by lws */
    long toWriteLimit = end - p - 1;

    if( messageSize == 0 || (int) ( messageSize - sessionData.bytesWritten() ) < (int) toWriteLimit )
        n = LWS_WRITE_HTTP_FINAL;
    if( error.empty() ) {
        size_t offset = m_endpointHandler->readFile( params, (char*) p, toWriteLimit );
        LOGGER_TRACE( "readFile: bytes read: " + cc::num10( offset ) );
        p += offset;
        LOGGER_TRACE_PRINT( "httpCallback / http_writeable: progress: %d/%u, portion: %u, wsi: %p",
                            (int) ( sessionData.bytesWritten() + offset ),
                            messageSize,
                            offset,
                            wsi );
    }
    else {
        p += lws_snprintf( (char*) p, end - p, "%s", error.c_str() );
    }
    auto toWrite = lws_ptr_diff( p, start );
    auto bytesWritten = lws_write( wsi, (uint8_t*) start, toWrite, static_cast< lws_write_protocol >( n ) );
    sessionData.addBytesWritten( bytesWritten );

    if( bytesWritten != toWrite ) {
        LOGGER_TRACE( "bytesWritten != toWrite" );
        return 1;
    }

    if( n == LWS_WRITE_HTTP_FINAL ) {
        if( lws_http_transaction_completed( wsi ) )
            return -1;
    }
    else if( !sessionData.isPending() ) {
        lws_callback_on_writable( wsi );
    }
    return 0;
}

int CamerasAddonEndpoint::onClosedHttp( CallbackParams params, BufferPosition /*bufPos*/ ) {
    LOGGER_TRACE_PRINT( "httpCallback / closed_http: wsi: %p", params.m_wsi );

    if( m_endpointHandler ) {
        m_endpointHandler->removeFile( params );
    }
    m_api.finishSession( params.m_wsi );
    return httpDummy( params );
}

size_t ICamerasEndpointHandler::getFileSize( CallbackParams params ) {
    std::lock_guard< std::mutex > lock( m_mutex );
    auto it = m_files.find( params.m_user );
    if( it == m_files.end() ) {
        LOGGER_DEBUG( "file not found" );
        return 0;
    }
    return it->second.fileSize;
}

size_t ICamerasEndpointHandler::readFile( CallbackParams params, char* dst, size_t size ) {
    std::lock_guard< std::mutex > lock( m_mutex );
    auto it = m_files.find( params.m_user );
    if( it == m_files.end() ) {
        LOGGER_DEBUG( "file not found" );
        return 0;
    }
    auto& file = it->second.file;
    if( file == nullptr || feof( file.get() ) ) {
        return 0;
    }
    return fread( dst, 1, size, file.get() );
}

void ICamerasEndpointHandler::removeFile( CallbackParams params ) {
    std::lock_guard< std::mutex > lock( m_mutex );
    m_files.erase( params.m_user );
}

void ICamerasEndpointHandler::callbackOnWritable( CallbackParams params ) {
    lws_callback_on_writable( params.m_wsi );
}

std::string ICamerasEndpointHandler::getErrorMessage( HttpBridge::CallbackParams params ) {
    std::lock_guard< std::mutex > lock( m_mutex );
    auto it = m_files.find( params.m_user );
    if( it == m_files.end() ) {
        return "no file found";
    }
    return it->second.error;
}

void ICamerasEndpointHandler::setErrorMessage( HttpBridge::CallbackParams params, const std::string& errorMessage ) {
    std::lock_guard< std::mutex > lock( m_mutex );
    m_files[ params.m_user ].error = errorMessage;
}

void EmptyCamerasEndpointHandler::getSnapshot( CallbackParams /*params*/, const std::string& /*streamId*/ ) {}

bool EmptyCamerasEndpointHandler::isHandlerValid() {
    return false;
}

}  // namespace HttpBridge
}  // namespace HubPlatform
