#include "uid/extensions/cameras/cameras_ui_http_extension.h"
#include "uid/extensions/ui_http_utils.h"
#include "common_utils/cpp14.h"

namespace HubPlatform {
namespace Cameras {

namespace {
struct AudioStreamRequestUserData : public Uid::Extensions::HttpSession::UserData {
    explicit AudioStreamRequestUserData( std::string streamId )
    : streamId( std::move( streamId ) ) {}

    std::string streamId;
};
}  // namespace

UiHttpExtension::UiHttpExtension( AudioStreamManager& manager )
: m_manager( manager ) {}

int UiHttpExtension::onHttp( struct lws* wsi, Uid::Extensions::HttpSession& session ) {
    static std::string audioStreamPrefix = "/stream/audio/";
    if( strncmp( session.url.c_str(), audioStreamPrefix.c_str(), audioStreamPrefix.size() ) != 0 ) {
        return Uid::Extensions::Utils::writeError( wsi, 404, "Cameras addon does not provide specified functionality" );
    }

    std::string streamId = session.url.substr( audioStreamPrefix.size() );
    if( streamId.empty() ) {
        return Uid::Extensions::Utils::writeError( wsi, 400, "Stream id not specified" );
    }

    LOGGER_DEBUG_RAW( cc::debug( "Got connection for input audio stream " ) + cc::note( streamId ) );

    auto stream = m_manager.getInputAudioStream( streamId );
    if( !stream ) {
        LOGGER_DEBUG_RAW( cc::debug( "Failed to find input audio stream " ) + cc::note( streamId ) );
        return Uid::Extensions::Utils::writeError( wsi, 404, "Stream with specified id not found" );
    }

    if( !stream->isRunning() ) {
        LOGGER_DEBUG_RAW( cc::debug( "Requested input audio stream " ) + cc::note( streamId ) +
                          cc::debug( " is closed" ) );
        return Uid::Extensions::Utils::writeError( wsi, 400, "Stream is closed" );
    }

    if( !stream->startDataSending() ) {
        LOGGER_DEBUG_RAW( cc::debug( "Failed to initialize data transmission for input audio stream " ) +
                          cc::note( streamId ) );
        return Uid::Extensions::Utils::writeError( wsi, 400, "Failed to initialize data transmission" );
    }

    session.userData = std::make_unique< AudioStreamRequestUserData >( std::move( streamId ) );

    if( Log::getLogLevel() >= Log::PriorityLevel::TRACE ) {
        std::string headers;
        for( const auto& header : session.headers ) {
            headers += header.first + " : " + header.second + "\n";
        }
        LOGGER_TRACE_RAW( cc::trace( "Headers:\n" ) + cc::note( headers ) );
    }

    if( session.hasHeader( "expect", "100-continue" ) ) {
        // Write 100 continue
        uint8_t buf[ LWS_PRE + LWS_RECOMMENDED_MIN_HEADER_SPACE ];
        uint8_t *start = &buf[ LWS_PRE ], *p = start, *end = &buf[ sizeof( buf ) - 1 ];
        lws_add_http_header_status( wsi, 100, &p, end );
        lws_finalize_write_http_header( wsi, start, &p, end );
        lws_callback_on_writable( wsi );
    }

    return 0;
}

int UiHttpExtension::onHttpBody( struct lws* wsi,
                                 Uid::Extensions::HttpSession& session,
                                 const char* data,
                                 size_t size ) {
    if( !session.userData )
        return 1;

    auto userDataPtr = dynamic_cast< AudioStreamRequestUserData* >( session.userData.get() );
    if( !userDataPtr )
        return 1;

    auto streamPtr = m_manager.getInputAudioStream( userDataPtr->streamId );
    if( !streamPtr )
        return 1;

    if( !streamPtr->send( data, size ) ) {
        LOGGER_DEBUG_RAW( cc::debug( "Failed to send new data for input audio stream " ) +
                          cc::note( userDataPtr->streamId ) );
        return 1;
    }

    return 0;
}

int UiHttpExtension::onHttpBodyCompletion( struct lws* wsi, Uid::Extensions::HttpSession& session ) {
    if( !session.userData ) {
        return 1;
    }

    auto userDataPtr = dynamic_cast< AudioStreamRequestUserData* >( session.userData.get() );
    if( !userDataPtr )
        return 1;

    LOGGER_DEBUG_RAW( cc::debug( "Got all data for input audio stream " ) + cc::note( userDataPtr->streamId ) );

    // Write simple 200 response
    uint8_t buf[ LWS_PRE + LWS_RECOMMENDED_MIN_HEADER_SPACE ];
    uint8_t *start = &buf[ LWS_PRE ], *p = start, *end = &buf[ sizeof( buf ) - 1 ];
    lws_add_http_header_status( wsi, 200, &p, end );
    session.responseHeadersWritten = true;
    lws_finalize_write_http_header( wsi, start, &p, end );
    lws_callback_on_writable( wsi );

    return session.hasHeader( "connection", "keep-alive" ) ? 0 : 2;
}

int UiHttpExtension::onHttpClosed( struct lws* wsi, Uid::Extensions::HttpSession& session ) {
    if( !session.userData ) {
        return 1;
    }

    auto userDataPtr = dynamic_cast< AudioStreamRequestUserData* >( session.userData.get() );
    if( !userDataPtr )
        return 1;

    LOGGER_DEBUG_RAW( cc::debug( "Closing connection for input audio stream " ) + cc::note( userDataPtr->streamId ) );

    auto streamPtr = m_manager.getInputAudioStream( userDataPtr->streamId );
    if( !streamPtr )
        return 1;

    streamPtr->stopDataSending();

    return 0;
}

int UiHttpExtension::onHttpWriteable( struct lws* wsi, Uid::Extensions::HttpSession& session ) {
    return 0;
}

bool UiHttpExtension::authorizationRequired() const {
    return true;
}

int UiHttpExtension::onTimer( struct lws* wsi, Uid::Extensions::HttpSession& session ) {
    return 0;
}

}  // namespace Cameras
}  // namespace HubPlatform
