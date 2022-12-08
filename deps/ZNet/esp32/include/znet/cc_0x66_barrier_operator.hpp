#pragma once

#include <znet/znet_command_class.hpp>

namespace znet {

class ZCc0x66BarrierOperator : public ZClass {
public:
    using ID = ZClassID<ZNET_COMMAND_CLASS_BARRIER_OPERATOR>;
    using ZClass::ZClass;

    using SignalCallback = std::function<void( uint8_t )>;

    bool requestValue( const RequestValue& request ) override;

    bool interview( InterviewCallback callback, WeakJobContext context ) override;

    void setSignalValue( uint8_t signalId, uint8_t value );
    bool requestSignalValue( uint8_t signalId,
                             SignalCallback onSuccess = {},
                             OnFailCallback onFail = {},
                             WeakJobContext context = {} );

    const std::vector<uint8_t>& getSupportedSignals() const;

protected:
    bool setValue( const uint8_t value ) override;

    bool setToken( const std::string& value ) override;

    bool setTokenMulticast( const uint8_t nodeQuantity,
                            const znet_node_id_t* listNodeId,
                            const std::string& value,
                            bool sendSingleCasts ) override;

    bool storeData( IFile& file ) const override;
    bool restoreData( IFile& file, int version ) override;

    ZNET_DEFINE_ACCEPT()

private:
    void requestNextSignal( InterviewCallbackPtr callbackPtr, WeakJobContext jobContext, uint8_t index = 0 );

private:
    std::vector<uint8_t> m_supportedSignals;
};

}  // namespace znet
