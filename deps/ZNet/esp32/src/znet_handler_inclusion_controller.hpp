#pragma once

#include "znet_handler_interview.hpp"

namespace znet {

class InclusionHandler : public InterviewHandler {
protected:
    static const uint64_t WaitingInitiateMs;
    static const uint64_t WaitingDeviceInfoMs;
    static const uint64_t WaitingSecurityMs;
    static const uint64_t WaitingNodeUpdateMs;
    static const uint64_t WaitingSecuritySupportedMs;
    static const uint64_t StopWaiting;
    static const uint64_t WaitingInterviewFinishedMs;

    enum class InclusionStep {
        Ready = 0,
        WaitingInitiate,
        NodeInfoWaiting,
        WaitingSecurity,
        WaitingNodeUpdate,
        WaitingSecuritySupported,
        WaitingS0InclutionComplete,
        WaitingInterviewFinished,
        /**
         * In case if an Initiate Command (WaitingInitiate step) has timed out we MUST
         * start the interview process in unsecured mode.
         */
        WaitingNodeInfoToStartUnsecuredInterview,
        /**
         * In case if an inclusion controller does not support S0 we take
         * responsibility for S0 bootstrapping and start by requesting a NIF.
         * It is not explicitly forbidden by the protocol, so we are gonna choose
         * highest available security level.
         */
        WaitingNodeInfoToStartS0Bootstrapping,
        Done
    };

public:
    InclusionHandler( ZNetImpl& parent );

    ~InclusionHandler() = default;
    void initialize() override;

    bool onLoop() override;

    bool canHandleNode( znet_node_id_t nodeId ) const override;

    void onApplicationNodeUpdate( const znet_nodeinfo_t* nodeInfo ) override;

    void onNodeInclusionControllerInitiate( znet_node_id_t nodeId,
                                            const znet_node_id_t childNodeId,
                                            const znet_inclution_controller_step_id_t stepId ) override;

    void onNodeInclusionControllerComplete( znet_node_id_t nodeId,
                                            const znet_inclution_controller_step_id_t stepId,
                                            const znet_inclution_controller_status_t status ) override;

    void onNodeCmdSecuritySupported( const uint8_t nodeId,
                                     const znet_security_supported_report_t* nodeInfoSecuritySupported ) override;

    void executeOneStep( const Step interviewStep, const uint8_t nodeId ) override;

    void onNodeAdd( const znet_nodeinfo_t* nodeInfo ) override;

protected:
    znet_basic_class_t m_nodeBasic;
    InclusionStep m_inclusionStep = InclusionStep::Ready;
};

}  // namespace znet
