#include "znet/znet_requests_manager.hpp"
#include "znet_requests_manager_impl.ipp"

namespace znet {

template BaseRequestsManager::RequestKey::RequestKey( const RequestKey& other );

template BaseRequestsManager::RequestKey::~RequestKey();

template bool BaseRequestsManager::RequestKey::operator<( const RequestKey& other ) const;

template BaseRequestsManager::~BaseRequestsManager();

template RequestFailHandlingResult BaseRequestsManager::onFailImpl( const RequestKey&, RequestFailType, bool );

template void BaseRequestsManager::eraseExpiredHandlers( std::vector<RespondHandlingInfo>& );

template bool BaseRequestsManager::doRequestImpl( RequestKey&& key,
                                                  const std::chrono::milliseconds& firstDelay,
                                                  const WeakJobContext& jobContext,
                                                  RequestsManagerHelpers::RespondCallbackWrapperUPtr&& onSuccessCb,
                                                  OnFailCb&& onFailCb,
                                                  size_t maxRetryNumber,
                                                  size_t maxFailNumber );

template void BaseRequestsManager::onRequestSentImpl( RequestKey&& key );

template void BaseRequestsManager::onRespondImpl( RequestKey&& key, RespondData&& respond );

template bool BaseRequestsManager::sendRequest( const RequestKey& key );

}  // namespace znet
