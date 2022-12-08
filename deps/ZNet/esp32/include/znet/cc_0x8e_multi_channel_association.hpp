#pragma once
#include <znet/znet_command_class.hpp>

#include <unordered_map>

namespace znet {

class ZCc0x8eMultiChannelAssociation final : public ZClass {
public:
    using ID = ZClassID<ZNET_COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION>;
    using ZClass::ZClass;

    enum class NodeAddResult { Success, AlreadyExists, Failed };

    bool interview( InterviewCallback callback, WeakJobContext context ) override;

    void onGroupInfo( const znet_multichannel_association_report_t* value );

    uint8_t getAssociationGroupsCount() const;
    const std::vector<znet_endpoint_t>* getGroupInfo( uint8_t groupId ) const;
    uint8_t getGroupMaxNodesCount( uint8_t groupId ) const;

    NodeAddResult addNodeToGroup( uint8_t groupId,
                                  uint8_t nodeId,
                                  uint8_t targetChannelId,
                                  bool force = false,
                                  OnFailCallback onFail = {},
                                  WeakJobContext context = {} );

    bool removeNodeFromGroup( uint8_t groupId, uint8_t nodeId, uint8_t targetChannelId );

    bool requestValue( const RequestValue& request ) override;

    bool storeData( IFile& file ) const override;
    bool restoreData( IFile& file, int version ) override;

    ZNET_DEFINE_ACCEPT()

private:
    static const uint8_t ms_undefinedGroupsCount = 0;

    bool requestNextGroupInterview( WeakJobContext jobContext, RequestValue::CallbackPtr cb, uint8_t groupId );
    bool requestNextGroup( WeakJobContext jobContext, RequestValue::CallbackPtr cb, uint8_t groupId );

    bool setSisToLifelineGroup( WeakJobContext jobContext, RequestValue::CallbackPtr cb );

    void completeInterviewProcess( RequestValue::CallbackPtr cb );

    uint8_t m_associationsCount = ms_undefinedGroupsCount;
    uint8_t m_currentProcessingGroup = ms_undefinedGroupsCount;  // Legacy

    std::vector<uint8_t> m_groupsMaxNodesCount;
    std::unordered_map<uint8_t, std::vector<znet_endpoint_t>> m_groupInfos;
};

}  // namespace znet
