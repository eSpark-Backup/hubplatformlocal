#pragma once
#include <znet/znet_node.hpp>
#include <znet/znet_command_class.hpp>
#include <vector>

namespace znet {

class ZCc0x60MultiChannel final : public ZClass {
public:
    using ID = ZClassID<ZNET_COMMAND_CLASS_MULTI_CHANNEL>;
    using ZClass::ZClass;

    bool buildChannels( InterviewCallback callback, WeakJobContext jobContext );

    ZNET_DEFINE_ACCEPT()

private:
    void processChannelsFind( InterviewCallbackPtr callbackPtr, WeakJobContext context, bool doRequest = true );

    void processNextChannel( InterviewCallbackPtr callbackPtr,
                             WeakJobContext context,
                             znet_node_channel_id_t channelId = 1 );

    void addCommandClasses( znet_node_channel_id_t channelId, const znet_command_class_t* classes, uint8_t size );
};

}  // namespace znet
