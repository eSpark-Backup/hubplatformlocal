#pragma once

#include <znet/znet_command_class.hpp>
#include <znet/optional.hpp>
#include <functional>

namespace znet {

class ZCc0x86Version final : public ZClass {
public:
    using ID = ZClassID<ZNET_COMMAND_CLASS_VERSION>;
    using ZClass::ZClass;

    ZCc0x86Version( ZChannel& parent, znet_command_class_version_t version = 1 );
    ~ZCc0x86Version() override = default;

    bool interview( InterviewCallback callback, WeakJobContext context ) override;

    bool storeData( IFile& file ) const override;
    bool restoreData( IFile& file, int version ) override;

    ZNET_DEFINE_ACCEPT();

    bool requestCommandClassVersion( znet_command_class_t classId,
                                     std::function<void( znet_command_class_version_t )> onSuccess = {},
                                     OnFailCallback onFail = {},
                                     WeakJobContext jobContext = {},
                                     bool shortRequest = false );

    bool isUpToDate() const;

    utils::optional<std::string> getLibraryType() const;
    utils::optional<uint8_t> getHardwareVersion() const;

    // The format is major.minor
    utils::optional<std::string> getZwaveProtocolVersion() const;

    uint8_t getFirmwaresCount() const;
    std::string getFirmwareVersion( uint8_t idx = 0 ) const;

    // The format is [major.minor.patch] - empty string means the field is not used
    utils::optional<std::string> getSdkVersion() const;
    utils::optional<std::string> getApplicationFrameworkApiVersion() const;
    utils::optional<std::string> getHostInterfaceVersion() const;
    utils::optional<std::string> getProtocolVersion() const;
    utils::optional<std::string> getApplicationVersion() const;

    // Note: 0 means the field is not used
    utils::optional<uint16_t> getApplicationFrameworkApiBuildNumber() const;
    utils::optional<uint16_t> getHostInterfaceBuildNumber() const;
    utils::optional<uint16_t> getProtocolBuildNumber() const;
    utils::optional<uint16_t> getApplicationBuildNumber() const;

private:
    static utils::optional<std::string> versionToString( const znet_version_t& version );
    static std::string versionToString( const znet_version_minor_major_t& version );

    void processVersionReport( const znet_version_report_t& versionReport );
    void processSoftwareVersionReport( const znet_software_version_report_t& softwareVersionReport );

private:
    enum class LibraryType {
        StaticController = 0x01,
        Controller = 0x02,
        EnhancedSlave = 0x03,
        Slave = 0x04,
        Installer = 0x05,
        RoutingSlave = 0x06,
        BridgeController = 0x07,
        DeviceUnderTest = 0x08,
        AVRemote = 0x0A,
        AVDevice = 0x0B,

        Unknown = 0xFF
    };

    struct SoftwareVersionInfo {
        znet_version_t sdkVersion;
        znet_version_t applicationFrameworkApiVersion;
        znet_version_t hostInterfaceVersion;
        znet_version_t protocolVersion;
        znet_version_t applicationVersion;
        uint16_t applicationFrameworkApiBuildNumber;
        uint16_t hostInterfaceBuildNumber;
        uint16_t protocolBuildNumber;
        uint16_t applicationBuildNumber;
    };

private:
    LibraryType m_libraryType;
    utils::optional<znet_version_minor_major_t> m_zwaveProtocolVersion;
    utils::optional<uint8_t> m_hardwareVersion;
    std::vector<znet_version_minor_major_t> m_firmwareVersions;
    utils::optional<SoftwareVersionInfo> m_softwareVersionInfo;
};

}  // namespace znet
