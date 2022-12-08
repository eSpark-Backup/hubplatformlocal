#include "znet_fc_send_data_mock.h"
#include <gtest/gtest.h>

extern "C" {
#include <znet_lib_fc.h>
}

namespace znet {

namespace {
FcSendDataMock* instance = nullptr;
znet_fc_send_data_t origSendData = nullptr;
}  // namespace

FcSendDataMock::FcSendDataMock() {
    if( instance ) {
        throw std::logic_error( "FcSendDataMock instance is not nullptr" );
    }

    instance = this;
    origSendData = znet_fc_send_data;
    znet_fc_send_data = []( uint8_t node_id,
                            uint8_t channel_id,
                            uint8_t length,
                            const uint8_t* data,
                            ZFunction_Callback callback,
                            void* callbackArg,
                            encap_type_t encapType ) {
        return instance->fcSendData(
            node_id, channel_id, std::vector<uint8_t>( data, data + length ), !!callback, !!callbackArg, encapType );
    };
}

FcSendDataMock::~FcSendDataMock() {
    EXPECT_EQ( instance, this );
    znet_fc_send_data = origSendData;
    instance = nullptr;
}

}  // namespace znet
