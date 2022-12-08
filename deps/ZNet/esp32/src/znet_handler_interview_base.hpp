#pragma once

#include "znet_handler.hpp"
#include "znet/znet.hpp"
#include "znet/optional.hpp"

#include "znet/iznet_node_setting.hpp"

namespace znet {

class InterviewHandlerBase : public ZNetHandler {
    static const int DefaultAssociationGroupId;
    static const int DefaultMinWakeUpInterval;
    static const int SendWaitingTimeoutMs;
    static const int OneStepWaitingTimeoutMs;
    static const int SecurityWaitingTimeoutMs;
    static const int OneClassProcessingWaitingTimeoutMs;
    static const int InterviewTimeoutMs;
    static const int InclusionControllerTimeoutMs;
    static const int PrimaryControllerTimeoutMs;

public:
    enum class Step : uint8_t {
        Ready = 0,
        Started,
        Timeout,
        Finished,
        GetNodeInfo,
        WaitSecurity,
        GetManufactureSpecific,
        GetCommandClassVersion,
        GetZWavePlusInfo,
        WakeUpCapabilities,
        MultichannelAssociation,
        Association,
        AssociationGroupInfo,
        MultichannelEndpointSurvey,
        CommandClassesProcessing,
        SetupNodeSetting,
        WaitInclusionController,
        WaitPrimaryController
    };

    InterviewHandlerBase( ZNetImpl& parent );
    ~InterviewHandlerBase() = default;
    void initialize() override;

    bool onLoop() override;

    void onNodeCmdSecuritySupported( const uint8_t nodeId, const znet_security_supported_report_t* value ) override;

    void onNodeFailed( const uint8_t nodeId, int error ) override;

    void onWakeupNotification( uint8_t nodeId ) override;

    virtual void executeOneStep( const Step interviewStep, const uint8_t nodeId );

    virtual void sendInclusionProgress( const Step interviewStep );

    Step getCurrentStep() const;

protected:
    bool isAllowedCC( uint8_t classId ) const;
    bool isActuatorCC( uint8_t classId ) const;

    void onInterviewFinished( uint8_t command, uint8_t channel );

    virtual WeakJobContext getJobContext() const = 0;

    /**
     * Defines behaviour of storeSmartStartData: store nodeId only (NodeId), security modes only (SecurityModes) or full
     * Smart Start data available in nodeInfo (All).
     */
    enum class StoreSmartStartDataMode { NodeId, SecurityModes, All };
    /**
     * Checks whether a given nodeInfo contains Smart Start data and, if so, stores this data to the Provisioning List.
     * @return Indicates whether the node was included in Smart Start mode and the information is successfully stored to
     * the Provisioning List.
     */
    bool storeSmartStartData( const znet_nodeinfo_t&, StoreSmartStartDataMode );

    static utils::optional<uint8_t> getProgressByStep( const Step interviewStep );

    void updateInterviewTimeStamp();

    static std::chrono::milliseconds getMaxWaitingTime( Step step );

    void getSupportedCommandClassVersions( znet_node_id_t nodeId, uint8_t classIndex = 0, bool shortRequest = false );
    void getUnsupportedCommandClassVersions( znet_node_id_t nodeId, uint8_t classIndex = 0, bool shortRequest = false );

    void getManufacturerSpecifics( znet_node_id_t nodeId );

    void getZwavePlusInfo( znet_node_id_t nodeId );

    void getWakeUpCapabilities( znet_node_id_t nodeId );

    void buildMultiChannels( znet_node_id_t nodeId );

    void processCommandClasses( znet_node_id_t nodeId, znet_node_channel_id_t channelId = 0, uint8_t ccIndex = 0 );
    void processUnsupportedCommandClasses( znet_node_id_t nodeId,
                                           znet_node_channel_id_t channelId = 0,
                                           uint8_t ccIndex = 0 );

    void getMultichannelAssociations( znet_node_id_t nodeId );

    void getAssociations( znet_node_id_t nodeId );

    void getAssociationsGroupInfo( znet_node_id_t nodeId );

    virtual void setupNodeSetting( znet_node_id_t nodeId ) = 0;

    void setupNodeSettingImpl( znet_node_id_t nodeId, bool setupAssociations, bool setupConfigurations );

    void updateInitialWokenUpState( ZNode& node );

protected:
    Step m_step;

    uint8_t m_nodeId = 0;
    std::unique_ptr<INodeSetting> m_nodeSetting;

    timeout_t m_interviewTimeStamp;
};

}  // namespace znet
