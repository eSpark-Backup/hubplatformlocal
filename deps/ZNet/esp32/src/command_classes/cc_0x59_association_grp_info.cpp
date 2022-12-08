#include <znet/znet_node.hpp>
#include <znet/cc_0x59_association_grp_info.hpp>
#include <znet/cc_0x85_association.hpp>
#include <znet/cc_0x8e_multi_channel_association.hpp>
#include <znet/znet_requests_manager.hpp>

#include <algorithm>
#include <cassert>

extern "C" {
#include <znet_log.h>
}

namespace znet {

ZCc0x59AssociationGrpInfo::ZCc0x59AssociationGrpInfo( ZChannel& parent, znet_command_class_version_t version )
: ZClass( parent, version )
, m_associationsCount( ms_undefinedGroupsCount )
, m_currentProcessingGroup( ms_undefinedGroupsCount )
, m_interviewState( InterviewState::Idle ) {}

bool ZCc0x59AssociationGrpInfo::interview( InterviewCallback callback, WeakJobContext context ) {
    m_associationsCount = getAssociationsCount( getParentChannel().getId() );

    if( m_associationsCount > 0 ) {
        auto callbackPtr = std::make_shared<InterviewCallback>( std::move( callback ) );
        return requestGroupName( context, callbackPtr, 1 );
    }

    return false;
}

uint8_t ZCc0x59AssociationGrpInfo::getAssociationGroupsCount() const {
    return m_associationsCount;
}

bool ZCc0x59AssociationGrpInfo::hasGroup( uint8_t groupId ) const {
    return findInfo( groupId ) != nullptr;
}

const std::string* ZCc0x59AssociationGrpInfo::getAssociationGroupName( uint8_t groupId ) const {
    if( const auto* info = findInfo( groupId ) ) {
        return &info->name;
    }

    return nullptr;
}

const ZCc0x59AssociationGrpInfo::GroupProfile* ZCc0x59AssociationGrpInfo::getAssociationGroupProfile(
    uint8_t groupId ) const {
    if( const auto* info = findInfo( groupId ) ) {
        return &info->profile;
    }

    return nullptr;
}

const ZCc0x59AssociationGrpInfo::CommandsInfo* ZCc0x59AssociationGrpInfo::getAssociationGroupCommands(
    uint8_t groupId ) const {
    if( const auto* info = findInfo( groupId ) ) {
        return &info->commands;
    }

    return nullptr;
}

bool ZCc0x59AssociationGrpInfo::storeData( IFile& file ) const {
    bool result = file.write( &m_associationsCount, sizeof( m_associationsCount ) );
    result &= file.write( &m_currentProcessingGroup, sizeof( m_currentProcessingGroup ) );
    result &= file.write( &m_interviewState, sizeof( m_interviewState ) );

    int groups_count = m_groupInfos.size();
    result &= file.write( &groups_count, sizeof( groups_count ) );
    for( auto const& groupInfo : m_groupInfos ) {
        result &= file.write( &groupInfo.first, sizeof( groupInfo.first ) );

        const GroupDescription& data = groupInfo.second;

        int nameSize = data.name.size() + 1;  // Null terminator
        char const* cName = data.name.c_str();
        result &= file.write( &nameSize, sizeof( nameSize ) );
        result &= file.write( cName, sizeof( char ) * nameSize );

        result &= file.write( &data.profile.first, sizeof( data.profile.first ) );
        result &= file.write( &data.profile.second, sizeof( data.profile.second ) );

        int commandsCount = data.commands.size();
        result &= file.write( &commandsCount, sizeof( commandsCount ) );
        for( const auto& command : data.commands ) {
            result &= file.write( &command.first, sizeof( command.first ) );
            result &= file.write( &command.second, sizeof( command.second ) );
        }
    }

    return result;
}

bool ZCc0x59AssociationGrpInfo::restoreData( IFile& file, int version ) {
    bool result = file.read( &m_associationsCount, sizeof( m_associationsCount ) );
    result &= file.read( &m_currentProcessingGroup, sizeof( m_currentProcessingGroup ) );
    result &= file.read( &m_interviewState, sizeof( m_interviewState ) );

    int groups_count;
    result &= file.read( &groups_count, sizeof( groups_count ) );
    for( int i = 0; i < groups_count; ++i ) {
        uint8_t groupNum;
        result &= file.read( &groupNum, sizeof( groupNum ) );

        m_groupInfos[groupNum] = GroupDescription();

        char buf[43];
        int nameSize;
        result &= file.read( &nameSize, sizeof( nameSize ) );
        result &= file.read( buf, sizeof( char ) * nameSize );
        m_groupInfos[groupNum].name = std::string( buf );

        std::pair<uint8_t, uint8_t> profile;
        result &= file.read( &profile.first, sizeof( profile.first ) );
        result &= file.read( &profile.second, sizeof( profile.second ) );
        m_groupInfos[groupNum].profile = profile;

        int commandsCount;
        result &= file.read( &commandsCount, sizeof( commandsCount ) );
        for( int j = 0; j < commandsCount; ++j ) {
            std::pair<uint8_t, uint8_t> command;
            result &= file.read( &command.first, sizeof( command.first ) );
            result &= file.read( &command.second, sizeof( command.second ) );
            m_groupInfos[groupNum].commands.push_back( command );
        }
    }

    return result;
}

uint8_t ZCc0x59AssociationGrpInfo::getAssociationsCount( const znet_node_channel_id_t channelId ) const {
    ZNode::ClassPtr ccTempMultichannelAssociations =
        getParentNode().getCommandClass( channelId, ZNET_COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION );
    if( ccTempMultichannelAssociations && ccTempMultichannelAssociations->hasInterviewDone() ) {
        ZCc0x8eMultiChannelAssociation* cc =
            static_cast<ZCc0x8eMultiChannelAssociation*>( ccTempMultichannelAssociations.get() );
        return cc->getAssociationGroupsCount();
    }

    ZNode::ClassPtr ccTempAssociations = getParentNode().getCommandClass( channelId, ZNET_COMMAND_CLASS_ASSOCIATION );
    if( ccTempAssociations ) {
        ZCc0x85Association* cc = static_cast<ZCc0x85Association*>( ccTempAssociations.get() );
        return cc->getAssociationGroupsCount();
    }

    return ms_undefinedGroupsCount;
}

const ZCc0x59AssociationGrpInfo::GroupDescription* ZCc0x59AssociationGrpInfo::findInfo( uint8_t groupId ) const {
    if( groupId <= m_associationsCount ) {
        auto info = m_groupInfos.find( groupId );
        if( info != m_groupInfos.end() ) {
            return &info->second;
        }
    }

    return nullptr;
}

ZCc0x59AssociationGrpInfo::GroupIdOpt ZCc0x59AssociationGrpInfo::findAssociationGroup(
    znet_command_class_t ccId ) const {
    const auto found = std::find_if(
        m_groupInfos.begin(),
        m_groupInfos.end(),
        [ccId]( const std::pair<znet_grouping_id_t, GroupDescription>& groupPair ) {
            const auto& commands = groupPair.second.commands;
            return std::any_of( commands.begin(), commands.end(), [ccId]( const CommandInfo& commandInfo ) {
                return ccId == commandInfo.first;
            } );
        } );

    return found == m_groupInfos.end() ? GroupIdOpt( 0, false ) : GroupIdOpt( found->first, true );
}

bool ZCc0x59AssociationGrpInfo::requestGroupName( WeakJobContext jobContext,
                                                  RequestValue::CallbackPtr cb,
                                                  uint8_t groupId ) {
    return getParentNode().getRequestsManager().doRequest(
        AssociationGrpNameRequest( getNodeId(), getChannelId(), groupId ),
        jobContext,
        [=]( const znet_association_group_name_report_t* report ) {
            m_groupInfos[groupId] = GroupDescription();
            m_groupInfos[groupId].name = report->name;
            requestGroupInfo( jobContext, cb, groupId );
        },
        std::bind( onRequestsFinished, cb, std::placeholders::_1 ),
        DefaultPostponedRequestDelay );
}

bool ZCc0x59AssociationGrpInfo::requestGroupInfo( WeakJobContext jobContext,
                                                  RequestValue::CallbackPtr cb,
                                                  uint8_t groupId ) {
    return getParentNode().getRequestsManager().doRequest(
        AssociationGrpInfoRequest( getNodeId(), getChannelId(), groupId, 0, 1 ),
        jobContext,
        [=]( const znet_association_group_info_report_t* report ) {
            std::pair<uint8_t, uint8_t> profileData;
            profileData.first = report->group_infos[0].profile >> 8;
            profileData.second = report->group_infos[0].profile & 0xff;
            m_groupInfos[groupId].profile = profileData;

            requestGroupCommands( jobContext, cb, groupId );
        },
        std::bind( onRequestsFinished, cb, std::placeholders::_1 ),
        DefaultPostponedRequestDelay );
}

bool ZCc0x59AssociationGrpInfo::requestGroupCommands( WeakJobContext jobContext,
                                                      RequestValue::CallbackPtr cb,
                                                      uint8_t groupId ) {
    return getParentNode().getRequestsManager().doRequest(
        AssociationGrpCommandListRequest( getNodeId(), getChannelId(), groupId, 0 ),
        jobContext,
        [=]( const znet_association_group_command_list_report_t* report ) {
            for( int i = 0; i < report->commands_count; ++i ) {
                const auto& commandInfo = report->commands[i];
                m_groupInfos[groupId].commands.push_back(
                    std::make_pair( commandInfo.command, commandInfo.sub_command ) );
            }

            if( groupId < 255 && groupId + 1 <= m_associationsCount ) {
                requestGroupName( jobContext, cb, groupId + 1 );
            }
            else {
                setInterviewDone( true );
                onRequestsFinished( cb, {} );
            }
        },
        std::bind( onRequestsFinished, cb, std::placeholders::_1 ),
        DefaultPostponedRequestDelay );
}

}  // namespace znet
