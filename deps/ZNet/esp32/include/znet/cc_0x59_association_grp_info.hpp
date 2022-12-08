#pragma once
#include <znet/znet_command_class.hpp>

#include <unordered_map>

namespace znet {

class ZCc0x59AssociationGrpInfo final : public ZClass {
public:
    using CommandInfo = std::pair<znet_command_class_t, uint8_t>;
    using CommandsInfo = std::vector<CommandInfo>;
    using GroupProfile = std::pair<uint8_t, uint8_t>;

private:
    struct GroupDescription {
        std::string name;
        GroupProfile profile;
        CommandsInfo commands;
    };

    enum class InterviewState { Idle, NameGetting, InfoGetting, CommandsGetting };

public:
    using ID = ZClassID<ZNET_COMMAND_CLASS_ASSOCIATION_GRP_INFO>;

    using GroupIdOpt = std::pair<znet_grouping_id_t, bool>;

    ZCc0x59AssociationGrpInfo( ZChannel& parent, znet_command_class_version_t version = 1 );

    bool interview( InterviewCallback callback, WeakJobContext context ) override;

    uint8_t getAssociationGroupsCount() const;
    bool hasGroup( uint8_t groupId ) const;

    const std::string* getAssociationGroupName( uint8_t groupId ) const;

    const GroupProfile* getAssociationGroupProfile( uint8_t groupId ) const;

    const CommandsInfo* getAssociationGroupCommands( uint8_t groupId ) const;

    bool storeData( IFile& file ) const override;
    bool restoreData( IFile& file, int version ) override;

    GroupIdOpt findAssociationGroup( znet_command_class_t ccId ) const;

    ZNET_DEFINE_ACCEPT()

private:
    uint8_t getAssociationsCount( const znet_node_channel_id_t channelId ) const;

    const GroupDescription* findInfo( uint8_t groupId ) const;

    bool requestGroupName( WeakJobContext jobContext, RequestValue::CallbackPtr cb, uint8_t groupId );
    bool requestGroupInfo( WeakJobContext jobContext, RequestValue::CallbackPtr cb, uint8_t groupId );
    bool requestGroupCommands( WeakJobContext jobContext, RequestValue::CallbackPtr cb, uint8_t groupId );

    uint8_t m_associationsCount;
    uint8_t m_currentProcessingGroup;  // Legacy
    InterviewState m_interviewState;   // Legacy

    std::unordered_map<znet_grouping_id_t, GroupDescription> m_groupInfos;

    static const uint8_t ms_undefinedGroupsCount = 0;
};

}  // namespace znet
