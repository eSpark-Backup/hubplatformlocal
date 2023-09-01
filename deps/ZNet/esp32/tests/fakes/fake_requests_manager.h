#include <znet/znet_requests_manager.hpp>
#include <set>

namespace znet {
namespace ut {

class FakeRequestsManager : public RequestsManager {
public:
    using RequestsManager::RequestsManager;

    template <typename Request>
    void addFailedRequest( Request&& request ) {
        m_failedRequests.insert( RequestKey( std::forward<Request>( request ) ) );
    }

    void setUseFakeRequests( bool useFakeRequests ) {
        m_useFakeRequests = useFakeRequests;
    }

private:
    bool sendRequest( const RequestKey& key ) override {
        const auto it = m_failedRequests.find( key );
        if( it == m_failedRequests.end() ) {
            return m_useFakeRequests || RequestsManager::sendRequest( key );
        }

        m_failedRequests.erase( it );
        return false;
    }

private:
    std::multiset<RequestKey> m_failedRequests;
    bool m_useFakeRequests = true;
};

}  // namespace ut
}  // namespace znet
