#pragma once

#include <znet_handler_interview.hpp>

namespace znet {

class LearnHandler : public InterviewHandler {
    static const uint64_t WaitingSecurityMs;
    static const uint64_t WaitingFinishInterviewMs;
    static const uint64_t WaitingAssociationMs;
    static const uint64_t WaitingDeviceInfoMs;
    static const uint64_t StopWaiting;

protected:
    enum class LearnStep { Ready = 0, AssosiationSetFromSisWaiting, ListOfNodesSisWaiting, NodeInfoWaiting, Done };

public:
    LearnHandler( ZNetImpl& parent );

    ~LearnHandler() = default;
    void initialize() override;

    void onNodeAdd( const znet_nodeinfo_t* nodeInfo ) override;

    void onNodeLearn( const znet_node_id_t destNodeId ) override;

    void onNodeList( const uint8_t* nodesList ) override;

    void onNodeFailed( uint8_t nodeId, int error ) override;

    void onApplicationNodeUpdate( const znet_nodeinfo_t* nodeInfo ) override;

    void onNodeCmdSecuritySupported( const uint8_t nodeId,
                                     const znet_security_supported_report_t* nodeInfoSecuritySupported ) override;

    void executeOneStep( const Step interviewStep, const uint8_t nodeId ) override;

    void finalize( ErrorOpt ) override;

    void onAssociationSetResult( const uint8_t nodeId, const uint8_t channelId ) override;

    void onMultichannelAssociationSetResult( const uint8_t nodeId, const uint8_t channelId ) override;

    void onZwavePlusInfoGet() override;

    bool onLoop() override;

    bool canHandleNode( znet_node_id_t nodeId ) const override;

protected:
    bool startDeviceInterview();

    uint8_t m_destNodeId = 0;
    uint8_t m_nodeCounter;
    uint8_t m_devicesToLearnAmount = 0;
    uint8_t m_nodesList[ZNET_NODE_ID_MAX / 8];
    LearnStep m_learnStep = LearnStep::Ready;
};

}  // namespace znet
