#pragma once

#include <znet/znet.h>
#include <znet/IFile.hpp>
#include <znet/znet_command_class_visitor.hpp>
#include <znet/znet_request_value.hpp>
#include <znet/znet_command_class_value.hpp>
#include <znet/znet_requests_manager_defs.hpp>

#include <vector>
#include <memory>
#include <functional>

namespace znet {

class ZNode;
class ZChannel;
class IAssociationsConfigurator;

template <znet_command_class_t Value>
using ZClassID = std::integral_constant<znet_command_class_t, Value>;

constexpr std::chrono::milliseconds DefaultPostponedRequestDelay{
    20
};  // Use some little delay to ensure postponed request sending

class ZClass : public ZClassAcceptor {
    friend class CommandClassValuesSetter;

public:
    using OnFailCallback = std::function<void( RequestFailType )>;
    using InterviewCallback = std::function<void( RequestFailTypeOpt )>;
    using InterviewCallbackPtr = std::shared_ptr<InterviewCallback>;

    ZClass( ZChannel& parentChannel, const znet_command_class_version_t version = 1 );

    ZClass( const ZClass& cc ) = delete;
    ZClass( ZClass&& cc ) = delete;
    ZClass& operator=( const ZClass& cc ) = delete;
    ZClass& operator=( ZClass&& cc ) = delete;

    virtual ~ZClass() = default;

    znet_command_class_t getId() const;

    void setVersion( const znet_command_class_version_t value );

    znet_command_class_version_t getVersion() const;

    ZChannel& getParentChannel() const;

    ZNode& getParentNode() const;

    void setParentChannel( ZChannel& parentChannel );

    virtual bool interview( InterviewCallback callback, WeakJobContext context );

    virtual bool requestValue( const RequestValue& request );

    bool canRequest() const;

    bool hasInterviewDone() const;

    void setInterviewDone( const bool value );

    virtual bool storeData( IFile& file ) const;
    virtual bool restoreData( IFile& file, int version );

    virtual bool isRediscoverRequired() const;

protected:  // Don't use directly. Use CommandClassValuesSetter instead
    virtual bool setToken( const std::string& value );

    virtual bool setValue( const uint8_t value );

    virtual bool setValueMulticast( const uint8_t nodeQuantity,
                                    const znet_node_id_t* listNodeId,
                                    const uint8_t value,
                                    bool sendSingleCasts );

    virtual bool setTokenMulticast( const uint8_t nodeQuantity,
                                    const znet_node_id_t* listNodeId,
                                    const std::string& value,
                                    bool sendSingleCasts );

    virtual bool setRgb( ColorComponents& components );

    virtual bool setRgbMulticast( const uint8_t nodeQuantity,
                                  const znet_node_id_t* listNodeId,
                                  ColorComponents& components,
                                  bool sendSingleCasts );

protected:
    znet_node_id_t getNodeId() const;
    znet_node_channel_id_t getChannelId() const;

    template <typename CommandClass, typename Request>
    static bool requestValueImpl( CommandClass& cc,
                                  Request&& request,
                                  RequestValue::CallbackPtr cb,
                                  const WeakJobContext& jobContext ) {
        auto& node = cc.getParentNode();

        return node.getRequestsManager().doRequest( std::forward<Request>( request ),
                                                    jobContext,
                                                    std::bind( onRequestsFinished, cb, RequestFailTypeOpt{} ),
                                                    std::bind( onRequestsFinished, cb, std::placeholders::_1 ) );
    }

    template <typename CommandClass, typename Request>
    static bool requestValueImpl( CommandClass& cc, Request&& request, const RequestValue& requestValue ) {
        auto cb = requestValue.resultCallback ?
                      std::make_shared<RequestValue::Callback>( requestValue.resultCallback ) :
                      nullptr;

        return requestValueImpl( cc, std::forward<Request>( request ), std::move( cb ), requestValue.jobContext );
    }

    static void onRequestsFinished( RequestValue::CallbackPtr cb, RequestFailTypeOpt failTypeOpt ) {
        if( cb && *cb ) {
            ( *cb )( failTypeOpt );
        }
    }

    template <typename MainCb, typename FinalCb>
    struct FinalInterviewCallbackWrapper {
        template <typename... Args>
        void operator()( Args&&... args ) const {
            mainCb( std::forward<Args>( args )... );
            cmdClass.setInterviewDone( true );
            if( finalCb && *finalCb ) {
                ( *finalCb )( {} );
            }
        }

        ZClass& cmdClass;
        MainCb mainCb;
        std::shared_ptr<FinalCb> finalCb;
    };

    template <typename CommandClass, typename Request, typename MainCb, typename FinalCb>
    bool singleInterviewRequestImpl( CommandClass& cc,
                                     Request&& request,
                                     const WeakJobContext& jobContext,
                                     MainCb mainCb,
                                     FinalCb finalCb,
                                     size_t maxRequestRetryNumber = MaxRetryRequestNumber ) {
        auto finalCbPtr = std::make_shared<FinalCb>( std::move( finalCb ) );

        return cc.getParentNode().getRequestsManager().doRequest(
            std::forward<Request>( request ),
            jobContext,
            FinalInterviewCallbackWrapper<MainCb, FinalCb>{ *this, std::move( mainCb ), finalCbPtr },
            [finalCbPtr]( RequestFailType failType ) { onRequestsFinished( finalCbPtr, failType ); },
            std::chrono::milliseconds( 0 ),
            maxRequestRetryNumber );
    }

private:
    std::reference_wrapper<ZChannel> m_parentChannel;
    znet_command_class_version_t m_version;
    bool m_interviewDone = false;
};

}  // namespace znet
