#include "znet/cc_0x86_version.hpp"
#include "znet/znet_node.hpp"
#include "znet/znet_requests_manager.hpp"
#include <znet/znet.h>

extern "C" {
#include <znet_log.h>
}

namespace znet {

ZCc0x86Version::ZCc0x86Version( ZChannel& parent, znet_command_class_version_t version )
: ZClass( parent, version )
, m_libraryType{ LibraryType::Unknown } {}

bool ZCc0x86Version::interview( InterviewCallback callback, WeakJobContext context ) {
    auto callbackPtr = std::make_shared<InterviewCallback>( std::move( callback ) );

    auto onSucceded = [=]() {
        setInterviewDone( true );
        onRequestsFinished( callbackPtr, {} );
    };

    auto sendSoftwareVersionRequest = [=]() {
        getParentNode().getRequestsManager().doRequest(
            SoftwareVersionRequest( getNodeId() ),
            context,
            [=]( const znet_software_version_report_t& softwareVersionReport ) {
                processSoftwareVersionReport( softwareVersionReport );
                onSucceded();
            },
            std::bind( onRequestsFinished, callbackPtr, std::placeholders::_1 ),
            DefaultPostponedRequestDelay );
    };

    auto sendVersionCapabilitiesRequest = [=]() {
        getParentNode().getRequestsManager().doRequest(
            VersionCapabilitiesRequest( getNodeId() ),
            context,
            [=]( const znet_version_capabilities_report_t& capabilitiesReport ) {
                if( !capabilitiesReport.software_supported ) {
                    onSucceded();
                    return;
                }

                sendSoftwareVersionRequest();
            },
            std::bind( onRequestsFinished, callbackPtr, std::placeholders::_1 ),
            DefaultPostponedRequestDelay );
    };

    return getParentNode().getRequestsManager().doRequest(
        VersionRequest( getNodeId() ),
        context,
        [=]( const znet_version_report_t& versionReport ) {
            processVersionReport( versionReport );

            if( getVersion() < 3 ) {
                onSucceded();
                return;
            }

            sendVersionCapabilitiesRequest();
        },
        std::bind( onRequestsFinished, callbackPtr, std::placeholders::_1 ) );
}

bool ZCc0x86Version::storeData( IFile& file ) const {
    file.write( &m_libraryType, sizeof( uint8_t ) );

    const uint8_t zwaveProtocolVersionPresent = m_zwaveProtocolVersion.has_value();
    file.write( &zwaveProtocolVersionPresent, sizeof( uint8_t ) );
    if( zwaveProtocolVersionPresent ) {
        file.write( m_zwaveProtocolVersion.get(), sizeof( znet_version_minor_major_t ) );
    }

    const uint8_t hardwareVersionPresent = m_hardwareVersion.has_value();
    file.write( &hardwareVersionPresent, sizeof( uint8_t ) );
    if( hardwareVersionPresent ) {
        file.write( m_hardwareVersion.get(), sizeof( uint8_t ) );
    }

    const uint8_t firmwaresCount = static_cast<uint8_t>( m_firmwareVersions.size() );
    file.write( &firmwaresCount, sizeof( uint8_t ) );
    for( auto& firmwareVersion : m_firmwareVersions ) {
        file.write( &firmwareVersion, sizeof( znet_version_minor_major_t ) );
    }

    const uint8_t softwareVersionInfoPresent = m_softwareVersionInfo.has_value();
    file.write( &softwareVersionInfoPresent, sizeof( uint8_t ) );
    if( softwareVersionInfoPresent ) {
        file.write( m_softwareVersionInfo.get(), sizeof( SoftwareVersionInfo ) );
    }

    return true;
}

bool ZCc0x86Version::restoreData( IFile& file, int version ) {
    if( version < majorFormatVersion_v9 )
        return true;

    static_assert( sizeof( znet_version_minor_major_t ) == 2, "struct was updated - fix save/restore" );
    static_assert( sizeof( SoftwareVersionInfo ) == 24, "struct was updated - fix save/restore" );

    file.read( &m_libraryType, sizeof( uint8_t ) );
    if( version >= majorFormatVersion_v10 ) {
        uint8_t zwaveProtocolVersionPresent = 0;
        file.read( &zwaveProtocolVersionPresent, sizeof( uint8_t ) );
        if( zwaveProtocolVersionPresent ) {
            znet_version_minor_major_t zwaveProtocolVersion;
            file.read( &zwaveProtocolVersion, sizeof( znet_version_minor_major_t ) );
            m_zwaveProtocolVersion = zwaveProtocolVersion;
        }

        uint8_t hardwareVersionPresent = 0;
        file.read( &hardwareVersionPresent, sizeof( uint8_t ) );
        if( hardwareVersionPresent ) {
            uint8_t hardwareVersion;
            file.read( &hardwareVersion, sizeof( uint8_t ) );
            m_hardwareVersion = hardwareVersion;
        }
    }
    else {
        znet_version_minor_major_t zwaveProtocolVersion;
        file.read( &zwaveProtocolVersion, sizeof( znet_version_minor_major_t ) );
        m_zwaveProtocolVersion = zwaveProtocolVersion;

        uint8_t hardwareVersion;
        file.read( &hardwareVersion, sizeof( uint8_t ) );
        m_hardwareVersion = hardwareVersion;
    }

    uint8_t firmwaresCount = 0;
    file.read( &firmwaresCount, sizeof( uint8_t ) );
    for( uint8_t i = 0; i != firmwaresCount; ++i ) {
        znet_version_minor_major_t firmwareVersion;
        file.read( &firmwareVersion, sizeof( znet_version_minor_major_t ) );

        m_firmwareVersions.push_back( firmwareVersion );
    }

    uint8_t softwareVersionInfoPresent = 0;
    file.read( &softwareVersionInfoPresent, sizeof( uint8_t ) );
    if( softwareVersionInfoPresent ) {
        SoftwareVersionInfo softwareVersionInfo;
        file.read( &softwareVersionInfo, sizeof( SoftwareVersionInfo ) );

        m_softwareVersionInfo = std::move( softwareVersionInfo );
    }

    return true;
}

bool ZCc0x86Version::isUpToDate() const {
    return m_libraryType != LibraryType::Unknown;
}

utils::optional<std::string> ZCc0x86Version::getLibraryType() const {
    switch( m_libraryType ) {
    case LibraryType::StaticController:
        return utils::optional<std::string>( "static_controller" );

    case LibraryType::Controller:
        return utils::optional<std::string>( "controller" );

    case LibraryType::EnhancedSlave:
        return utils::optional<std::string>( "enhanced_slave" );

    case LibraryType::Slave:
        return utils::optional<std::string>( "slave" );

    case LibraryType::Installer:
        return utils::optional<std::string>( "installer" );

    case LibraryType::RoutingSlave:
        return utils::optional<std::string>( "routing_slave" );

    case LibraryType::BridgeController:
        return utils::optional<std::string>( "bridge_controller" );

    case LibraryType::DeviceUnderTest:
        return utils::optional<std::string>( "device_under_test" );

    case LibraryType::AVRemote:
        return utils::optional<std::string>( "av_remote" );

    case LibraryType::AVDevice:
        return utils::optional<std::string>( "av_device" );

    default:
        return utils::optional<std::string>();
    }
}

utils::optional<uint8_t> ZCc0x86Version::getHardwareVersion() const {
    return m_hardwareVersion;
}

utils::optional<std::string> ZCc0x86Version::getZwaveProtocolVersion() const {
    if( m_zwaveProtocolVersion )
        return versionToString( *m_zwaveProtocolVersion );
    else
        return utils::optional<std::string>();
}

uint8_t ZCc0x86Version::getFirmwaresCount() const {
    return static_cast<uint8_t>( m_firmwareVersions.size() );
}

std::string ZCc0x86Version::getFirmwareVersion( uint8_t idx ) const {
    return versionToString( m_firmwareVersions.at( idx ) );
}

utils::optional<std::string> ZCc0x86Version::getSdkVersion() const {
    return m_softwareVersionInfo ? versionToString( m_softwareVersionInfo->sdkVersion ) :
                                   utils::optional<std::string>();
}

utils::optional<std::string> ZCc0x86Version::getApplicationFrameworkApiVersion() const {
    return m_softwareVersionInfo ? versionToString( m_softwareVersionInfo->applicationFrameworkApiVersion ) :
                                   utils::optional<std::string>();
}

utils::optional<std::string> ZCc0x86Version::getHostInterfaceVersion() const {
    return m_softwareVersionInfo ? versionToString( m_softwareVersionInfo->hostInterfaceVersion ) :
                                   utils::optional<std::string>();
}

utils::optional<std::string> ZCc0x86Version::getProtocolVersion() const {
    return m_softwareVersionInfo ? versionToString( m_softwareVersionInfo->protocolVersion ) :
                                   utils::optional<std::string>();
}

utils::optional<std::string> ZCc0x86Version::getApplicationVersion() const {
    return m_softwareVersionInfo ? versionToString( m_softwareVersionInfo->applicationVersion ) :
                                   utils::optional<std::string>();
}

utils::optional<uint16_t> ZCc0x86Version::getApplicationFrameworkApiBuildNumber() const {
    return m_softwareVersionInfo ? m_softwareVersionInfo->applicationFrameworkApiBuildNumber :
                                   utils::optional<uint16_t>();
}

utils::optional<uint16_t> ZCc0x86Version::getHostInterfaceBuildNumber() const {
    return m_softwareVersionInfo ? m_softwareVersionInfo->hostInterfaceBuildNumber : utils::optional<uint16_t>();
}

utils::optional<uint16_t> ZCc0x86Version::getProtocolBuildNumber() const {
    return m_softwareVersionInfo ? m_softwareVersionInfo->protocolBuildNumber : utils::optional<uint16_t>();
}

utils::optional<uint16_t> ZCc0x86Version::getApplicationBuildNumber() const {
    return m_softwareVersionInfo ? m_softwareVersionInfo->applicationBuildNumber : utils::optional<uint16_t>();
}

utils::optional<std::string> ZCc0x86Version::versionToString( const znet_version_t& version ) {
    if( version.major == 0 && version.minor == 0 && version.patch == 0 )
        return {};

    char buffer[12] = { 0 };  // max length = 3 * 3-digit elements + 2 separators + 1 null terminator
    sprintf( buffer, "%d.%d.%d", version.major, version.minor, version.patch );

    return std::string( buffer );
}

std::string ZCc0x86Version::versionToString( const znet_version_minor_major_t& version ) {
    char buffer[8] = { 0 };  // max length = 2 * 3-digit elements + 1 separator + 1 null terminator
    sprintf( buffer, "%d.%d", version.ver, version.sub_ver );

    return { buffer };
}

void ZCc0x86Version::processVersionReport( const znet_version_report_t& versionReport ) {
    m_libraryType = static_cast<LibraryType>( versionReport.lib_type );
    m_zwaveProtocolVersion = { versionReport.proto.ver, versionReport.proto.sub_ver };
    m_hardwareVersion = versionReport.hard_ver;

    m_firmwareVersions.reserve( versionReport.firms_count );
    for( uint8_t i = 0; i != versionReport.firms_count; ++i ) {
        const auto& firmwareVersion = versionReport.firms[i];
        m_firmwareVersions.push_back( { firmwareVersion.ver, firmwareVersion.sub_ver } );
    }
}

void ZCc0x86Version::processSoftwareVersionReport( const znet_software_version_report_t& softwareVersionReport ) {
    m_softwareVersionInfo = { softwareVersionReport.sdk_version,
                              softwareVersionReport.application_framework_api_version,
                              softwareVersionReport.host_interface_version,
                              softwareVersionReport.protocol_version,
                              softwareVersionReport.application_version,
                              softwareVersionReport.application_framework_api_build_number,
                              softwareVersionReport.host_interface_build_number,
                              softwareVersionReport.protocol_build_number,
                              softwareVersionReport.application_build_number };
}

bool ZCc0x86Version::requestCommandClassVersion( znet_command_class_t classId,
                                                 std::function<void( znet_command_class_version_t )> onSuccess,
                                                 OnFailCallback onFail,
                                                 WeakJobContext jobContext,
                                                 bool shortRequest ) {
    return getParentNode().getRequestsManager().doRequest(
        CommandClassVersionRequest( getNodeId(), getChannelId(), classId ),
        jobContext,
        std::move( onSuccess ),
        std::move( onFail ),
        std::chrono::milliseconds( 0 ),
        shortRequest ? 1 : MaxRetryRequestNumber );
}

}  // namespace znet
