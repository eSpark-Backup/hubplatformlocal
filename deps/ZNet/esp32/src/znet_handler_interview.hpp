#pragma once

#include "znet_handler_interview_base.hpp"

namespace znet {

class InterviewHandler : public InterviewHandlerBase {
public:
    using InterviewHandlerBase::InterviewHandlerBase;

    void onNodeAdd( const znet_nodeinfo_t* node_info ) override;
    void onNodeAddSecurityFailed() override;
    void onNodeAddSecurityKeysRequested( znet_S2_keys_t ) override;
    void onNodeAddSecurityKeysGranted( znet_S2_keys_t ) override;
    void onNodeAddSecurityPubKeyRequested( const znet_S2_pub_key_t* ) override;
    void onNodeAddSecurityPubKeyGranted( const znet_S2_pub_key_t* ) override;
    void onNodeAddSecurityDone( SecurityMode::Class ) override;

    void startInclude( const DskDerivedHomeId& dskDerivedHomeId ) override;

    void stopInclude() override;

    void initialize() override;

    void finalize( ErrorOpt ) override;

    bool canHandleNode( znet_node_id_t nodeId ) const override;

    void setupNodeSetting( znet_node_id_t nodeId ) override;

    void onNodeCmdSecuritySupported( const uint8_t nodeId, const znet_security_supported_report_t* value ) override;

    void onNodeInclusionControllerInitiate( znet_node_id_t nodeId,
                                            const znet_node_id_t childNodeId,
                                            const znet_inclution_controller_step_id_t stepId ) override;

    void onNodeInclusionControllerComplete( znet_node_id_t nodeId,
                                            const znet_inclution_controller_step_id_t stepId,
                                            const znet_inclution_controller_status_t status ) override;

    void onApplicationNodeUpdate( const znet_nodeinfo_t* nodeInfo ) override;

protected:
    WeakJobContext getJobContext() const override;

    void updateJobContext();

    void clearJobContext();

private:
    JobContextPtr m_interviewJobcontext;
};

}  // namespace znet
