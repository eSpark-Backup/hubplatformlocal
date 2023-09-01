#include "common_utils/cpp14.h"
#include "bus/messagesclasses.h"
#include "service_interfaces/groups_names.h"
#include "bus/spread_bus.h"
#include "hardware_manager/config/hw_manager_config.h"
#include "hardware_manager/effect/update_ezlofw_effect.h"
#include "hardware_manager/effect/hub_reboot_effect.h"
#include "hardware_manager/effect/firmware_down_preset.h"
#include "hardware_manager/hub_state/model/hub_state.h"

using namespace std;
using namespace HubPlatform;
using namespace HubPlatform::HardwareManager;

namespace {

const char* const FIRMWARE_UPDATE_PROCESS = "firmwareUpdate";
const char* const HUB_REBOOT_PROCESS = "hubReboot";

void help() {
    const char* const progName = "ha-ledcmd";
    const char* const sp4 = "    ";
    cerr << "Usage:" << endl;
    cerr << sp4 << progName << " --start process" << endl;
    cerr << sp4 << progName << " --stop process" << endl;
    cerr << "Options:" << endl;
    cerr << sp4 << "Processes: " << FIRMWARE_UPDATE_PROCESS << ", " << HUB_REBOOT_PROCESS << endl;
}

class Environment {
public:
    Environment()
    : m_bus( nullptr, []( BaseBus* spread ) {} ) {}

    LedColorPwm& getEmotionLed() {
        return getHardwareConfig().getEmotionLed();
    }

    bool onProcessRunning( HubState::Process process ) {
        auto busPtr = getBus();
        if( !busPtr ) {
            return false;
        }
        busPtr->send( GroupsNames::HARDWARE_MANAGER,
                      MessageClasses::JSON_RPC,
                      HUB_PROCESS_RUNNING,
                      JSON( "type" << HubState::toString( process ) ) );
        return true;
    }

    bool onProcessStopped( HubState::Process process,
                           HubState::ProcessResult result = HubState::ProcessResult::Unspecified ) {
        auto busPtr = getBus();
        if( !busPtr ) {
            return false;
        }
        busPtr->send( GroupsNames::HARDWARE_MANAGER,
                      MessageClasses::JSON_RPC,
                      HUB_PROCESS_STOPPED,
                      JSON( "type" << HubState::toString( process ) << "result" << HubState::toString( result ) ) );
        return true;
    }

private:
    HWManagerConfig& getHardwareConfig() {
        if( !m_config ) {
            m_config = std::make_unique< HWManagerConfig >();
        }
        return *m_config;
    }

    BaseBus* getBus() {
        if( system( "pidof ha-hardware_manager > /dev/null 2>&1" ) != 0 ) {
            return nullptr;
        }

        if( !m_bus ) {
            try {
                m_bus = createBus( "led_command", []( const bus_rx_data_t* /*rx_data*/ ) {}, true, 1 );
                m_bus->start();
            }
            catch( const std::runtime_error& error ) {
            }
        }
        return m_bus.get();
    }

private:
    std::unique_ptr< HWManagerConfig > m_config;
    BaseBusPtr m_bus;
};

}  // namespace

int main( int argc, char* argv[] ) {
    if( argc < 3 ) {
        cerr << "Wrong params count: " << argc << "." << endl;
        help();
        return 1;
    }

    using namespace HubPlatform::HardwareManager;
    Environment env;

    if( string( argv[ 1 ] ) == "--start" ) {
        std::string preset{ argv[ 2 ] };
        if( preset == FIRMWARE_UPDATE_PROCESS ) {
            if( !env.onProcessRunning( HubState::Process::FirmwareUpdate ) ) {
                updateEzloFWEffectPreset( env.getEmotionLed() );
            }
        }
        else if( preset == HUB_REBOOT_PROCESS ) {
            if( !env.onProcessRunning( HubState::Process::Reboot ) ) {
                hubRebootEffectPreset( env.getEmotionLed() );
            }
        }
        else {
            cout << "Unknown preset: " << preset << std::endl;
            help();
            return 1;
        }
        return 0;
    }
    else if( string( argv[ 1 ] ) == "--stop" ) {
        std::string preset{ argv[ 2 ] };
        if( preset == FIRMWARE_UPDATE_PROCESS ) {
            if( !env.onProcessStopped( HubState::Process::FirmwareUpdate ) ) {
                firmwareDownPreset( env.getEmotionLed() );
            }
        }
        else if( preset == HUB_REBOOT_PROCESS ) {
            if( !env.onProcessStopped( HubState::Process::Reboot ) ) {
                firmwareDownPreset( env.getEmotionLed() );
            }
        }
        else {
            cout << "Unknown preset: " << preset << std::endl;
            help();
            return 1;
        }
        return 0;
    }
    return 0;
}
