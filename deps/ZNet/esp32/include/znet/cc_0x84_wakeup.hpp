#pragma once

#include <znet/znet_command_class.hpp>
#include <znet/optional.hpp>

namespace znet {

class ZCc0x84WakeUp final : public ZClass {
public:
    using ID = ZClassID<ZNET_COMMAND_CLASS_WAKE_UP>;
    using ZClass::ZClass;

    static const uint32_t DefaultWakeUpInterval;

    bool setWakeUpInterval( uint32_t wakeupInterval, bool changeCacheImmediatelly = true );

    bool interview( InterviewCallback callback, WeakJobContext context ) override;

    bool requestValue( const RequestValue& request ) override;

    bool requestWakeupInterval( std::function<void( uint32_t, znet_node_id_t )> onSuccess,
                                OnFailCallback onFail = {},
                                WeakJobContext context = {} );

    uint32_t normalizeWakeUpInterval( uint32_t wakeupInterval ) const;

    bool storeData( IFile& file ) const override;
    bool restoreData( IFile& file, int version ) override;

    uint32_t getMaxWakeUpInterval() const;
    uint32_t getMinWakeUpInterval() const;
    uint32_t getStepWakeUpInterval() const;
    uint32_t getInterval() const;
    bool hasCapabilities() const;

    ZNET_DEFINE_ACCEPT()

    void onIntervalReceived( uint32_t interval, znet_node_id_t targetNodeId );

private:
    void setWakeUpCapabilities( uint32_t minInterval, uint32_t maxInterval, uint32_t step );

private:
    uint32_t m_maxWakeUpInterval = 0;
    uint32_t m_minWakeUpInterval = 0;
    uint32_t m_step = 0;
    bool m_hasCapabilities = false;

    uint32_t m_wakeupInterval = 0;
};

}  // namespace znet
