#pragma once

#include <znet/znet_requests_manager.hpp>
#include <sstream>

extern "C" {
#include "znet_log.h"
}

namespace znet {

namespace RequestCbUtils {

template <size_t Index,
          typename Request,
          bool Enabled = true,
          bool End = Index == std::tuple_size<typename Request::Data>::value>
struct EnableForUnspecified {};

template <size_t Index, typename Request, bool Enabled>
struct EnableForUnspecified<Index, Request, Enabled, false>
: std::enable_if<Enabled == IsUnspecifiedParam<typename std::tuple_element<Index, typename Request::Data>::type>::value,
                 Request> {};

template <size_t Index, typename Request>
struct EnableForLast : std::enable_if<Index == std::tuple_size<typename Request::Data>::value, Request> {};

template <size_t SourceIndex, size_t Delta, typename Request, typename SourceArgs, typename... RequestArgs>
typename EnableForUnspecified<SourceIndex, Request>::type makeRequestImpl( SourceArgs&&, RequestArgs&&... );

template <size_t SourceIndex, size_t Delta, typename Request, typename SourceArgs, typename... RequestArgs>
typename EnableForUnspecified<SourceIndex, Request, false>::type makeRequestImpl( SourceArgs&&, RequestArgs&&... );

template <size_t SourceIndex, size_t Delta, typename Request, typename SourceArgs, typename... RequestArgs>
typename EnableForLast<SourceIndex, Request>::type makeRequestImpl( SourceArgs&&, RequestArgs&&... );

template <size_t SourceIndex, size_t Delta, typename Request, typename SourceArgs, typename... RequestArgs>
typename EnableForLast<SourceIndex, Request>::type makeRequestImpl( SourceArgs&&, RequestArgs&&... requestArgs ) {
    return Request( std::forward<RequestArgs>( requestArgs )... );
}

template <size_t SourceIndex, size_t Delta, typename Request, typename SourceArgs, typename... RequestArgs>
typename EnableForUnspecified<SourceIndex, Request, false>::type makeRequestImpl( SourceArgs&& sourceArgs,
                                                                                  RequestArgs&&... requestArgs ) {
    return makeRequestImpl<SourceIndex + 1, Delta, Request>(
        std::forward<SourceArgs>( sourceArgs ),
        std::forward<RequestArgs>( requestArgs )...,
        std::get<SourceIndex - Delta>( std::forward<SourceArgs>( sourceArgs ) ) );
}

template <size_t SourceIndex, size_t Delta, typename Request, typename SourceArgs, typename... RequestArgs>
typename EnableForUnspecified<SourceIndex, Request>::type makeRequestImpl( SourceArgs&& sourceArgs,
                                                                           RequestArgs&&... requestArgs ) {
    return makeRequestImpl<SourceIndex + 1, Delta + 1, Request>(
        std::forward<SourceArgs>( sourceArgs ),
        std::forward<RequestArgs>( requestArgs )...,
        typename std::tuple_element<SourceIndex, typename Request::Data>::type( {} ) );
}

template <typename Request, typename... SourceArgs>
Request makeRequest( SourceArgs&&... sourceArgs ) {
    return makeRequestImpl<0, 0, Request>( std::forward_as_tuple( std::forward<SourceArgs>( sourceArgs )... ) );
}

void getErrorArgs( std::stringstream& stream, const uint8_t& value );

template <typename T>
void getErrorArgs( std::stringstream& stream, const T& value ) {
    stream << value;
}

inline void getErrorArgs( std::stringstream& stream ) {}

template <typename Arg, typename... Args>
typename std::enable_if<sizeof...( Args ) != 0>::type getErrorArgs( std::stringstream& stream,
                                                                    Arg&& first,
                                                                    Args&&... args ) {
    getErrorArgs( stream, std::forward<Arg>( first ) );
    stream << ", ";
    getErrorArgs( stream, std::forward<Args>( args )... );
}

void getErrorBegin( std::stringstream& stream,
                    RequestFailHandlingResult failHandlingResult,
                    const char* name,
                    znet_error_t err );

void getErrorEnd( std::stringstream& stream );

template <typename... RequestArgs>
std::string getError( RequestFailHandlingResult failHandlingResult,
                      const char* name,
                      znet_error_t err,
                      RequestArgs&&... args ) {
    std::stringstream stream;

    getErrorBegin( stream, failHandlingResult, name, err );
    getErrorArgs( stream, std::forward<RequestArgs>( args )... );
    getErrorEnd( stream );

    return stream.str();
}

}  // namespace RequestCbUtils

template <typename...>
struct ZNetCbWrapper;

template <typename Request, typename... RequestArgs, typename... RespondArgs, typename Delegate>
struct ZNetCbWrapper<Request, std::tuple<RequestArgs...>, std::tuple<RespondArgs...>, Delegate> {
    static void cb( znet_error_t err,
                    RequestArgs... requestArgs,
                    typename std::conditional<std::is_reference<RespondArgs>::value,
                                              typename std::add_pointer<RespondArgs>::type,
                                              RespondArgs>::type... resondArgs,
                    void* arg ) {
        if( !arg ) {
            ZNET_LOGE( "Invalid znet callback arg\n" );
            return;
        }

        Request request = RequestCbUtils::makeRequest<Request>( std::move( requestArgs )... );

        switch( err ) {
        case ZNET_ERR_OK:
            Delegate::getRequestsManager( arg ).onRespond( std::move( request ),
                                                           utils::derefIfNeeded<RespondArgs>( resondArgs )... );
            break;
        case ZNET_ERR_REQUEST_SENT_OK:
            Delegate::getRequestsManager( arg ).onRequestSent( std::move( request ) );
            break;
        default: {
            const bool forceFail = err == ZNET_ERR_NOMEM || err == ZNET_ERR_CALLBACK;
            const auto failHandlingResult =
                Delegate::getRequestsManager( arg ).onFail( std::move( request ), forceFail );
            if( failHandlingResult != RequestFailHandlingResult::RequestContinued ) {
                Delegate::template onRequestFailReported( arg, requestArgs... );
            }

            ZNET_LOGE(
                "%s\n",
                RequestCbUtils::getError( failHandlingResult, Request::getName(), err, requestArgs... ).c_str() );
        } break;
        }
    }
};

template <typename Request, typename Delegate>
struct ZNetCbWrapper<Request, Delegate>
: ZNetCbWrapper<Request,
                typename utils::RemoveArgs<IsUnspecifiedParam, std::tuple, typename Request::Data>::type,
                typename Request::Respond,
                Delegate> {};

template <typename Cb, Cb cb1, Cb cb2>
struct ZNetCbCombiner;

template <typename... Args, void ( *cb1 )( Args... ), void ( *cb2 )( Args... )>
struct ZNetCbCombiner<void ( * )( Args... ), cb1, cb2> {
    static void cb( Args... args ) {
        cb1( args... );
        cb2( args... );
    }
};

#define CombineZNetCb( CB1, CB2 ) ZNetCbCombiner<decltype( &CB1 ), &CB1, &CB2>::cb

template <typename Cb, Cb baseCb>
struct DefaultCbWrapper;

template <typename... Args, void ( *baseCb )( znet_error_t, Args... )>
struct DefaultCbWrapper<void ( * )( znet_error_t err, Args... ), baseCb> {
    static void cb( znet_error_t err, Args... args ) {
        if( err == ZNET_ERR_REQUEST_SENT_OK ) {
            return;
        }

        constexpr size_t LastArg = sizeof...( Args ) - 1;

        // TODO: may be cast last parm to ZNetImpl

        static_assert( std::is_same<typename std::tuple_element<LastArg, std::tuple<Args...>>::type, void*>::value,
                       "Last param mast be void*" );

        if( std::get<LastArg>( std::tie( args... ) ) == nullptr ) {
            return;
        }

        baseCb( err, args... );
    }
};

}  // namespace znet
