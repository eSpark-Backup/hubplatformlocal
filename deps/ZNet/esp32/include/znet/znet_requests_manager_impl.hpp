#pragma once

#include <znet/znet_requests_manager_defs.hpp>
#include <znet/iznet_timer_provider.hpp>
#include <znet/znet_job.hpp>
#include <znet/znet_request_fail.hpp>
#include <znet/variant.hpp>

#include <map>
#include <cassert>
#include <vector>

namespace znet {

namespace RequestsManagerHelpers {

template <typename...>
struct RespondCallbackWrapper;

template <>
struct RespondCallbackWrapper<> {
    virtual ~RespondCallbackWrapper() = default;
};

template <typename Function>
struct RespondCallbackWrapper<Function> : RespondCallbackWrapper<> {
    RespondCallbackWrapper( Function&& func )
    : m_function( std::move( func ) ) {}
    Function m_function;
};

using RespondCallbackWrapperUPtr = std::unique_ptr<RespondCallbackWrapper<>>;

struct RespondHandlingInfo {
    WeakJobContext m_jobContext;
    RespondCallbackWrapperUPtr m_onSuccessCb;
    std::function<void( RequestFailType )> m_onFailCb;
};

enum class RequestState : uint8_t { Initial, RequestSending, RespondWaiting };

struct RequestCheckingItem {
    ITimerProvider::TimerUPtr m_timer;
    size_t m_retryNumber = 0;
    size_t m_failNumber = 0;
    RequestState m_requestState = RequestState::Initial;
    std::vector<RespondHandlingInfo> m_respondHandlers;
};

}  // namespace RequestsManagerHelpers

template <typename... Requests>
class RequestsManagerImpl {
public:
    using This = RequestsManagerImpl<Requests...>;

protected:
    using RespondHandlingInfo = RequestsManagerHelpers::RespondHandlingInfo;
    using RequestCheckingItem = RequestsManagerHelpers::RequestCheckingItem;

    using OnFailCb = std::function<void( RequestFailType )>;

    using RespondData = utils::variant<typename Requests::Respond...>;

    // following definition can be used but it increases compilation time
    // using RespondData = typename utils::EnsureElementsUnique<  utils::variant, utils::variant<typename
    // Requests::Respond...> >::type;

    class RequestKey : private utils::variant<Requests...> {
    public:
        using Base = utils::variant<Requests...>;
        using Base::Base;

        const Base& getBase() const {
            return *this;
        }

        RequestKey( const RequestKey& other );
        // lib size economy: dont't compile move constructor ( anyway actually it's not strongly required )
        //  RequestKey( RequestKey&& other ) noexcept( std::is_nothrow_move_constructible< Base >::value );

        ~RequestKey();

        bool operator<( const RequestKey& other ) const;
    };

public:
    RequestsManagerImpl( ITimerProvider& timerProvider )
    : m_timerProvider( timerProvider ) {}

    ITimerProvider& getTimerProvider() {
        return m_timerProvider;
    }

    virtual ~RequestsManagerImpl();

    template <typename RequestType>
    bool doRequest( RequestType request,
                    const WeakJobContext& jobContext,
                    typename RequestType::RespondCallback onSuccessCb,
                    OnFailCb onFailCb,
                    std::chrono::milliseconds firstDelay = std::chrono::milliseconds( 0 ),
                    size_t maxRetryNumber = MaxRetryRequestNumber,
                    size_t maxFailNumber = MaxFailNumber ) {
        using Request = typename std::remove_reference<RequestType>::type;
        static_assert( utils::HasType<Request, Requests...>::value, "Invalid request" );

        return doRequestImpl(
            RequestKey( request ),
            firstDelay,
            jobContext,
            RequestsManagerHelpers::RespondCallbackWrapperUPtr(
                new RequestsManagerHelpers::RespondCallbackWrapper<typename RequestType::RespondCallback>(
                    std::move( onSuccessCb ) ) ),
            std::move( onFailCb ),
            maxRetryNumber,
            maxFailNumber );
    }

    template <typename Request>
    void onRequestSent( Request request ) {
        static_assert( utils::HasType<Request, Requests...>::value, "Invalid request" );

        onRequestSentImpl( RequestKey( std::move( request ) ) );
    }

    template <typename Request, typename... RespondArgs>
    void onRespond( Request request, RespondArgs&&... respondArgs ) {
        static_assert( utils::HasType<Request, Requests...>::value, "Invalid request" );

        onRespondImpl( RequestKey( std::move( request ) ),
                       typename Request::Respond( std::forward<RespondArgs>( respondArgs )... ) );
    }

    template <typename Request>
    bool isWaitingForRespond( Request request ) const {
        static_assert( utils::HasType<Request, Requests...>::value, "Invalid request" );

        return m_requests.find( RequestKey( std::move( request ) ) ) != m_requests.end();
    }

    template <typename Request>
    RequestFailHandlingResult onFail( Request request, bool isCritical = true ) {
        static_assert( utils::HasType<Request, Requests...>::value, "Invalid request" );

        return onFailImpl( RequestKey( std::move( request ) ), RequestFailType::ReportedFail, isCritical );
    }

protected:
    bool doRequestImpl( RequestKey&& key,
                        const std::chrono::milliseconds& firstDelay,
                        const WeakJobContext& jobContext,
                        RequestsManagerHelpers::RespondCallbackWrapperUPtr&& onSuccessCb,
                        OnFailCb&& onFailCb,
                        size_t maxRetryNumber,
                        size_t maxFailNumber );

    RequestFailHandlingResult onFailImpl( const RequestKey& requestKey,
                                          RequestFailType failType,
                                          bool isCritical = true );

    void onRespondImpl( RequestKey&& key, RespondData&& respondData );

    void onRequestSentImpl( RequestKey&& requestKey );

    static void eraseExpiredHandlers( std::vector<RespondHandlingInfo>& handlers );

    virtual bool sendRequest( const RequestKey& key );

private:
    ITimerProvider& m_timerProvider;
    std::map<RequestKey, RequestCheckingItem> m_requests;
};

}  // namespace znet
