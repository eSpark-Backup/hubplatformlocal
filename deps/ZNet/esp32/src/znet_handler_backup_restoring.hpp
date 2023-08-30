#pragma once

#include <znet_handler_monitoring_base.hpp>

#include <znet/znet_io_wrapper.hpp>
#include <znet/iznet_timer_provider.hpp>

namespace znet {

class BackupRestoringHandler : public ZNetHandler {
public:
    using ZNetHandler::ZNetHandler;

    bool setSource( IOWrapper::SourceUPtr source );

    bool isDirty() const;

    void initialize() override;

    void onDeactivated() override;

private:
    void openSession();

    void writeDataRequest( uint16_t offset, bool expectedEof, uint16_t checksum );

    void closeSession( ErrorOpt = {} );

    void onFinished( ErrorOpt = {} );

private:
    IOWrapper::SourceUPtr m_source;
    ITimerProvider::TimerUPtr m_timer;
};

}  // namespace znet
