#include "test_utils/component_tests/mqtt_broker/mqtt_broker.h"

#include "test_utils/test_environment.h"
#include "system_utils/filesystem.h"

#include <signal.h>

namespace HubPlatform {
namespace Tests {

MQTTBroker::MQTTBroker( std::string binaryPath )
: m_processForker( Process::Forker::getInstance() )
, m_binaryPath( std::move( binaryPath ) ) {
    if( m_binaryPath.empty() )
        m_binaryPath = Tests::getBuildBinFolder() + "/mosquitto";
    if( !filesystem::isExist( m_binaryPath ) )
        throw MQTTBrokerError( "MQTTBroker binary path does not exist: " + m_binaryPath );
}

MQTTBroker::~MQTTBroker() {
    stop();
}

void MQTTBroker::start( size_t port, Process::Forker::OnProcessCompletion onFinished ) {
    m_port = port;
    std::string command = m_binaryPath + " -p " + std::to_string( m_port );
    try {
        m_pid = m_processForker->fork( command.c_str(), std::move( onFinished ) );
    }
    catch( const Process::ForkError& e ) {
        throw MQTTBrokerError( e.what() );
    }
}

void MQTTBroker::stop() {
    if( m_pid != 0 )
        Process::Forker::kill( m_pid, SIGTERM );
}

std::string MQTTBroker::getURI() {
    return "127.0.0.1:" + std::to_string( m_port );
}

}  // namespace Tests
}  // namespace HubPlatform
