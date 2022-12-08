#pragma once

#include "znet_handler.hpp"

#include <znet/znet_firmware_update.hpp>
#include <list>

namespace znet {

class ZCc0x7AFirmwareUpdate;

class FirmwareUpdateHandler : public ZNetHandler {
public:
    FirmwareUpdateHandler( ZNetImpl& parent );

    struct TargetInfo {
        znet_node_id_t nodeId;
        uint8_t target;
        FirmwareUpdate::SourceUPtr source;
    };

    void addTarget( znet_node_id_t nodeId, uint8_t target, FirmwareUpdate::SourceUPtr source );

    bool isDirty() const;

    bool isInProgress() const;

    void initialize() override;

    void finalize( ErrorOpt ) override;

    void onDeactivated() override;

    bool onLoop() override;

    bool canHandleNode( znet_node_id_t nodeId ) const override;

    void onNodeCmdFirmwareUpdateMdRequestReport( znet_node_id_t nodeId,
                                                 znet_node_channel_id_t channelId,
                                                 FirmwareUpdate::UpdateMdRequestReportStatus status ) override;

    void onNodeCmdFirmwareUpdateMdRequested( znet_node_id_t nodeId,
                                             znet_node_channel_id_t channelId,
                                             uint8_t requestedReportsCount,
                                             uint16_t reportNumber ) override;

    void onNodeCmdFirmwareUpdateMdStatusReport( znet_node_id_t nodeId,
                                                znet_node_channel_id_t channelId,
                                                FirmwareUpdate::DataSendingStatus status,
                                                uint16_t waitingTime ) override;

    void onNodeFirmwareUpdateMdActivationStatusReport( znet_node_id_t nodeId,
                                                       znet_node_channel_id_t channelId,
                                                       uint8_t target,
                                                       const znet_firmware_info_t& firmware,
                                                       FirmwareUpdate::ActivationStatus status,
                                                       Uint8Opt hardwareId ) override;

private:
    void doStep();

    std::shared_ptr<ZCc0x7AFirmwareUpdate> getFirmwareUpdateCc( znet_node_id_t nodeId,
                                                                znet_node_channel_id_t channelId,
                                                                const char* method );

private:
    std::list<TargetInfo> m_targets;
    znet_node_id_t m_currentNode = 0;
};

}  // namespace znet
