#include "znet/znet_requests_manager_impl.hpp"

#include <algorithm>

namespace znet {

namespace {
struct DefaultRespondDoer {
    const RequestsManagerHelpers::RespondCallbackWrapper<>& cb;
    template < typename... Args >
    void operator () (const std::tuple< Args... >& respondData) const {
        auto& wrapperImpl = static_cast< const RequestsManagerHelpers::RespondCallbackWrapper< std::function< void(Args...) > >& >(cb);
        if ( wrapperImpl.m_function ) {
            utils::applyTuple( wrapperImpl.m_function, respondData );
        }
    }
};

struct DefaultRequestDoer {
    template < typename Request >
    bool operator () (const Request& request) const {
        typename Request::Doer doer;
        const typename Request::Data& data = static_cast<const typename Request::Data&>( request );
        return utils::applyTuple( doer, data );
    }
};
}

template <typename... Requests>
RequestsManagerImpl<Requests...>::RequestKey::RequestKey( const RequestKey& other ) : Base( other ) {}

template <typename... Requests>
RequestsManagerImpl<Requests...>::RequestKey::~RequestKey() {}

template <typename... Requests>
bool RequestsManagerImpl<Requests...>::RequestKey::operator < ( const RequestKey& other ) const {
    return Base::operator < ( other );
}

template <typename... Requests>
RequestsManagerImpl<Requests...>::~RequestsManagerImpl() {}

template <typename... Requests>
RequestFailHandlingResult RequestsManagerImpl<Requests...>::onFailImpl( const RequestKey& requestKey, RequestFailType failType, bool isCritical ) {
    auto found = m_requests.find( requestKey );

    if( found == m_requests.end() ) {
        return RequestFailHandlingResult::RequestNotFound;
    }

    if ( !isCritical && found->second.m_failNumber != 0 ) {
        --found->second.m_failNumber;
        found->second.m_requestState = RequestsManagerHelpers::RequestState::Initial;
        found->second.m_timer->runOnce( RequestCheckingPeriod );
        return RequestFailHandlingResult::RequestContinued;
    }

    auto handlers = std::move( found->second.m_respondHandlers );

    m_requests.erase( found );

    for( auto& handlingInfo : handlers ) {
        if( handlingInfo.m_onFailCb && handlingInfo.m_jobContext )
            handlingInfo.m_onFailCb(failType);
    }

    return RequestFailHandlingResult::RequestStopped;
}

template <typename... Requests>
void RequestsManagerImpl<Requests...>::eraseExpiredHandlers(std::vector<RespondHandlingInfo>& handlers) {
    handlers.erase( std::remove_if( handlers.begin(),
                                    handlers.end(),
                                    []( const RespondHandlingInfo& handlingInfo ) {
                                        return !handlingInfo.m_jobContext;
                                    } ),
                    handlers.end() );
}




template <typename... Requests>
bool RequestsManagerImpl<Requests...>::doRequestImpl( RequestKey&& key,
                                                      const std::chrono::milliseconds& firstDelay,
                                                      const WeakJobContext& jobContext,
                                                      RequestsManagerHelpers::RespondCallbackWrapperUPtr&& onSuccessCb,
                                                      OnFailCb&& onFailCb,
                                                      size_t maxRetryNumber,
                                                      size_t maxFailNumber ) {
    assert( jobContext );

    auto& requestCheckingItem = m_requests[key];
    requestCheckingItem.m_retryNumber = std::max(maxRetryNumber, requestCheckingItem.m_retryNumber);
    requestCheckingItem.m_failNumber = std::max(maxFailNumber, requestCheckingItem.m_failNumber);

    requestCheckingItem.m_respondHandlers.push_back( RespondHandlingInfo{
        jobContext,
        std::move(onSuccessCb),
        std::move(onFailCb) } );

    if( !requestCheckingItem.m_timer ) {
        requestCheckingItem.m_timer = m_timerProvider.createTimer( [this, key]() {
            auto found = m_requests.find( key );
            assert( found != m_requests.end() );

            if ( found->second.m_requestState == RequestsManagerHelpers::RequestState::RequestSending ) {
                onFailImpl( key, RequestFailType::SendingError );
                return;
            }

            auto& handlers = found->second.m_respondHandlers;
            eraseExpiredHandlers( handlers );

            if( handlers.empty() ) {
                m_requests.erase( found );
                return;
            }

            if( found->second.m_retryNumber == 0 ) {
                onFailImpl( key, RequestFailType::Timeout );
                return;
            }

            --found->second.m_retryNumber;

            found->second.m_requestState = RequestsManagerHelpers::RequestState::RequestSending;
            if( sendRequest( key ) ) {
                found->second.m_timer->runOnce( RequestSendingPeriod );
            }
            else {
                onFailImpl( key, RequestFailType::SendingError );
            }
        } );

        assert( requestCheckingItem.m_timer );

        if( firstDelay.count() ) {
            requestCheckingItem.m_timer->runOnce( firstDelay );
        }
        else if( sendRequest( key ) ) {
            requestCheckingItem.m_requestState = RequestsManagerHelpers::RequestState::RequestSending;
            requestCheckingItem.m_timer->runOnce( RequestSendingPeriod );
        }
        else {
            m_requests.erase( key );
            return false;
        }
    }
    // esle don't send request immediatelly (only reset counter)

    return true;
}

template <typename... Requests>
void RequestsManagerImpl<Requests...>::onRequestSentImpl( RequestKey&& requestKey ) {
    auto found = m_requests.find( requestKey );
    if ( found != m_requests.end() ) {
        found->second.m_requestState = RequestsManagerHelpers::RequestState::RespondWaiting;
        found->second.m_timer->runOnce( RequestCheckingPeriod );
    }
}


template <typename... Requests>
void RequestsManagerImpl<Requests...>::onRespondImpl( RequestKey&& key, RespondData&& data ) {
    auto found = m_requests.find( key );

    if( found != m_requests.end() ) {
        auto handlers = std::move( found->second.m_respondHandlers );

        m_requests.erase( found );

        for( auto& handlingInfo : handlers ) {
            if( handlingInfo.m_onSuccessCb && handlingInfo.m_jobContext ) {
                visitors::visit( DefaultRespondDoer{ *handlingInfo.m_onSuccessCb }, data );
            }
        }
    }
}

template <typename... Requests>
bool RequestsManagerImpl<Requests...>::sendRequest( const RequestKey& key ) {
    return visitors::visit( DefaultRequestDoer{}, key.getBase() );
}

}
