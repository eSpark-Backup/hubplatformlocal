#pragma once

#include <znet/znet.h>
#include <typedefs.h>

#include <gmock/gmock.h>

namespace znet {

class FcSendDataMock {
public:
    FcSendDataMock();

    ~FcSendDataMock();

    MOCK_METHOD( znet_error_t,
                 fcSendData,
                 ( uint8_t node_id,
                   uint8_t channel_id,
                   std::vector<uint8_t> data,
                   bool hasCallback,
                   bool hasCallbackArg,
                   encap_type_t encap_type ),
                 () );
};

}  // namespace znet
