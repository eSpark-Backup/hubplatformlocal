#pragma once

#include <znet_handler_monitoring_base.hpp>

#include <znet/znet_io_wrapper.hpp>
#include <znet/iznet_timer_provider.hpp>

namespace znet {

class BackupCreationHandler : public MonitoringBaseHandler {
public:
    using MonitoringBaseHandler::MonitoringBaseHandler;

    bool setDestination( IOWrapper::DestinationUPtr destination );

    bool isDirty() const;

    void initialize() override;

    void onDeactivated() override;

private:
    void openSession();

    void readDataRequest( uint16_t offset, uint16_t fileSize, uint16_t checksum );

    void closeSession( ErrorOpt = {} );

    void onFinished( ErrorOpt = {} );

private:
    IOWrapper::DestinationUPtr m_destination;

    ITimerProvider::TimerUPtr m_timer;
};

}  // namespace znet
