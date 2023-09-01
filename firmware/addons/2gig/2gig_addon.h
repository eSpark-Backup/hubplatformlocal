#pragma once

#include "addon_interfaces/addon.h"

#include "service_interfaces/task_loop_timer.h"

namespace TwoGig {

struct TwoGig;
struct Client;
struct InstrumentationClient;
class DevicesRepository;

}  // namespace TwoGig

namespace HubPlatform {
namespace TwoGig {

class DevicesRepositoryImpl;

class Addon final : public Addons::Addon {
    using Base = Addons::Addon;

public:
    static const char* const Name;
    static const char* const Version;

public:
    Addon( const std::string& devicePath, const std::string& storageFilePath, const std::string& resetPinGpio );
    ~Addon() override;

    CommonUtils::AddonVersion getVersion() const override;

    int restoreBackup( const std::string& tempBackupFolder ) override;

protected:
    void setUp() override;
    void tearDown() override;

    void handleMethodCall( const JsonObject& json, MessageSubClasses requestType, const std::string& sender ) override;
    JsonObject handleRequestFromBus( const JsonObject& request, MessageSubClasses requestType ) override;

private:
    void reset( const JsonObject& params );

    void startDeviceDiscovering( const JsonObject& params );
    void stopDeviceDiscovering( const JsonObject& params );

    void addDevice( const JsonObject& params );
    void removeDevice( const JsonObject& params );

    void configureDevice( const JsonObject& params );

    void migrateOldDevicesStorage( DevicesRepositoryImpl& newStorage );

private:
    const std::string m_devicePath;
    const std::string m_oldStorageFilePath;
    const std::string m_resetPinGpio;

    std::unique_ptr< ::TwoGig::TwoGig > m_instance;

    std::unique_ptr< ::TwoGig::Client > m_client;
    std::unique_ptr< ::TwoGig::InstrumentationClient > m_instrumentationClient;
    std::unique_ptr< ::TwoGig::DevicesRepository > m_devices;

    std::unique_ptr< Services::TaskLoopTimer > m_discoveringTimeoutTask;
};

}  // namespace TwoGig
}  // namespace HubPlatform
