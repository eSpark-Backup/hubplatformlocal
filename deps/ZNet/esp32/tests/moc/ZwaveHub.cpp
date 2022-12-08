#include <memory>
#include "utils/plog.h"
#include "utils/ZwaveErrors.h"
#include "ZwaveHub.h"
#include "DataFrame.h"
#include "Defs.h"
#include "fc/AddNodeToNetwork.h"
#include "fc/ApplicationNodeInformation.h"
#include "fc/Command.h"
#include "fc/DefaultCommandHolder.h"
#include "fc/GetNodeProtocolInfo.h"
#include "fc/GetSUCNodeID.h"
#include "fc/MemoryGetID.h"
#include "fc/NVRGetValue.h"
#include "fc/SendData.h"
#include "fc/SendDataBridge.h"
#include "fc/SendDataMulti.h"
#include "fc/SendDataMultiBridge.h"
#include "fc/SetListenBeforeTalkThreshold.h"
#include "fc/SetSUCNodeID.h"
#include "fc/TypeLibrary.h"
#include "fc/LibraryVersion.h"

static const char* logtag = "ZwaveHub";
#define LOG_LOCAL_LEVEL P_LOG_DEBUG

ZwaveHub::ZwaveHub( std::uint32_t homeId, uint8_t nodeId )
: Zwavenode( homeId, nodeId )
, m_SUCNodeId( nodeId ){};

ZwaveHub::ZwaveHub( std::uint32_t homeId, uint8_t nodeId, uint8_t sisId )
: Zwavenode( homeId, nodeId )
, m_SUCNodeId( sisId ){};

void ZwaveHub::deviceAdd( std::shared_ptr<Device> device ) {
    ZwaveErrors::error( device != nullptr, "Error parameter: %s: %d", __FILE__, __LINE__ );
    m_deviceList[device->getNodeId()] = device;
}

Device& ZwaveHub::deviceGet( uint8_t node ) {
    ZwaveErrors::error(
        m_deviceList.find( node ) != m_deviceList.end(), "Node %u not found: %s: %d", node, __FILE__, __LINE__ );
    assert( m_deviceList[node] != nullptr );
    return *m_deviceList[node];
}

bool ZwaveHub::diviceInInclusionExist() {
    for( auto& item : m_deviceList ) {
        if( item.second->inclusionIsReady() ) {
            return true;
        }
    }
    return false;
}

Device& ZwaveHub::deviceInInclusionGet() {
    for( auto& item : m_deviceList ) {
        if( item.second->inclusionIsReady() ) {
            return *item.second;
        }
    }

    return m_defaultDevice;
}

void ZwaveHub::setSUCNodeId( uint8_t SUCNodeId ) {
    m_SUCNodeId = SUCNodeId;
}

uint8_t ZwaveHub::getSUCNodeId() {
    return m_SUCNodeId;
}

void ZwaveHub::setLibraryType( uint8_t libraryType ) {
    m_LibraryType = libraryType;
}

uint8_t ZwaveHub::getLibraryType() {
    return m_LibraryType;
}

std::vector<uint8_t> ZwaveHub::getLibraryVersion() {
    return m_LibraryVersion;
}

void ZwaveHub::setDskDerivedHomeId( const ::znet::DskDerivedHomeId& newHomeId ) {
    m_dskDerivedHomeId = newHomeId;
}

::znet::DskDerivedHomeId ZwaveHub::getDskDerivedHomeId() const {
    return m_dskDerivedHomeId;
}

void ZwaveHub::rxData( const void* data, uint8_t length ) {
    if( data != NULL && ( (uint8_t*)data )[0] == 0x06 && length == 1 ) {
        // TODO add check ACk
        m_canRead = true;
        return;
    }

    auto idat = static_cast<const uint8_t*>( data );
    std::vector<uint8_t> rxData( idat, idat + length );
    std::swap( rxData, m_rxData );

    DataFrame dataFrame = DataFrameGen::toDataFrame( m_rxData );

    switch( dataFrame.m_commandId ) {
    case zdef::FunctionClassId::ApplicationNodeInformation: {
        ApplicationNodeInformation anic( *this );
        anic.execute();
        break;
    }

    case zdef::FunctionClassId::ApplicationNodeInformationCommandClasses: {
        MemoryGetID memid( *this );
        memid.execute();
        break;
    }

    case zdef::FunctionClassId::NVRGetValue: {
        NVRGetValue nvrGetValue( *this, dataFrame.m_parameterPack );
        nvrGetValue.execute();
        break;
    }

    case zdef::FunctionClassId::AddNodeToNetwork: {
        AddNodeToNetwork addToNetwork( *this, dataFrame.m_parameterPack );
        addToNetwork.execute();
        break;
    }

    case zdef::FunctionClassId::SetSUCNodeID: {
        SetSUCNodeID setSucNodeId( *this, dataFrame.m_parameterPack );
        setSucNodeId.execute();
        break;
    }

    case zdef::FunctionClassId::SetThreshold: {
        SetThreshold setThreshold( *this, dataFrame.m_parameterPack );
        setThreshold.execute();
        break;
    }

    case zdef::FunctionClassId::GetSUCNodeID: {
        GetSUCNodeID gSUCnId( *this );
        gSUCnId.execute();
        break;
    }

    case zdef::FunctionClassId::SendData: {
        SendData sendData( *this, dataFrame.m_parameterPack );
        sendData.execute();
        break;
    }

    case zdef::FunctionClassId::SendDataBridge: {
        SendDataBridge sendDataBr( *this, dataFrame.m_parameterPack );
        sendDataBr.execute();
        break;
    }

    case zdef::FunctionClassId::SendDataMulti: {
        SendDataMulti sendDataMulti( *this, dataFrame.m_parameterPack );
        sendDataMulti.execute();
        SendDataMultiWasReceived();
        break;
    }

    case zdef::FunctionClassId::SendDataMultiBridge: {
        SendDataMultiBridge sendDataMultiBridge( *this, dataFrame.m_parameterPack );
        sendDataMultiBridge.execute();
        SendDataMultiBridgeWasReceived();
        break;
    }

    case zdef::FunctionClassId::GetNodeProtocolInfo: {
        GetNodeProtocolInfo gni( *this, dataFrame.m_parameterPack );
        gni.execute();
        break;
    }

    case zdef::FunctionClassId::TypeLibrary: {
        TypeLibrary typeLib( *this, dataFrame.m_parameterPack );
        typeLib.execute();
        break;
    }

    case zdef::FunctionClassId::LibraryVersion: {
        LibraryVersion libVersion( *this, dataFrame.m_parameterPack );
        libVersion.execute();
        break;
    }

    default: {
        DefaultCommandHolder defh( *this, dataFrame.m_commandId );
        defh.execute();
    }
    }

    m_rxData.clear();
    m_canRead = true;
}

uint8_t ZwaveHub::txData( void* data, uint8_t length ) {
    uint8_t readed = 0;
    while( readed < length ) {
        if( m_txqueue.empty() ) {
            break;
        }
        ( (uint8_t*)data )[readed] = m_txqueue.front();
        m_txqueue.pop();
        ++readed;
    }
    return readed;
}

void ZwaveHub::setAck() {
    m_txqueue.push( zdef::Ack::ACK );
}

void ZwaveHub::setTxdata( Msg&& msg ) {
    if( msg.isFinalize() == false ) {
        msg.finalize();
    }
    auto data = msg.getData();
    for( uint8_t byte : data ) {
        m_txqueue.push( byte );
    }
}

uint8_t ZwaveHub::getTxDataLength() {
    return m_txqueue.size();
}
