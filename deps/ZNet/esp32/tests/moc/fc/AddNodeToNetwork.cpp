#include "fc/AddNodeToNetwork.h"
#include <iostream>
#include <gtest/gtest.h>
#include "Defs.h"

AddNodeToNetwork::AddNodeToNetwork( ZwaveHub& hub, std::vector<uint8_t> const& pack )
: Command( zdef::FunctionClassId::AddNodeToNetwork, hub )
, m_pack( pack ){};

void AddNodeToNetwork::execute() {
    std::cout << "AddNodeToNetwork" << std::endl;
    getCommandInvoker().setAck();

    uint8_t bMode = m_pack[0] & ~( addMode::OPTION_NORMAL_POWER | addMode::OPTION_NETWORK_WIDE );
    uint8_t funcID = m_pack[1];

    if( !getCommandInvoker().diviceInInclusionExist() ) {
        return;
    }
    Device& device = getCommandInvoker().deviceInInclusionGet();

    switch( bMode ) {
    case addMode::HOME_ID: {
        ASSERT_EQ( m_pack.size(), 10 ) << "HOST->ZW: REQ | 0x4A | 0x08 | funcID | pDsk[0] | ... | pDsk[7]";
        ASSERT_THAT( std::vector<uint8_t>( &m_pack[2], &m_pack[10] ),
                     ::testing::ElementsAreArray( getCommandInvoker().getDskDerivedHomeId() ) );
        // No break here! Proceed to the standard flow once DSK derived Home ID is valid
    }
    case addMode::NODE_ANY: {
        uint8_t bSource = 0;
        uint8_t bLen = 0;
        {
            std::vector<uint8_t> setdataFrame{ funcID, nodeStatus::LEARN_READY, bSource, bLen };
            Msg msg( zdef::Type::Request, get_id() );
            msg.append( std::move( setdataFrame ) );
            getCommandInvoker().setTxdata( std::move( msg ) );
        }

        {
            std::vector<uint8_t> setdataFrame{ funcID, nodeStatus::NODE_FOUND, bSource, bLen };
            Msg msg( zdef::Type::Request, get_id() );
            msg.append( std::move( setdataFrame ) );
            getCommandInvoker().setTxdata( std::move( msg ) );
        }

        bSource = device.getNodeId();

        // ZW->HOST: REQ | 0x4A | funcID | bStatus | bSource | bLen | basic | generic | specific | cmdclasses[ ]
        {
            std::vector<uint8_t> setdataFrame{
                funcID,
                nodeStatus::ADDING_SLAVE,
                bSource,
                // TODO
                0x0b,  // bLen
                0x04,  // basic
                0x11,  // generic
                0x04,  // specific
                0x26,  // SWITCH_MULTILEVEL
                0x2b,  // SCENE_ACTIVATION
                0x2c,  // SCENE_ACTUATOR_CONF
                0x27,  // SWITCH_ALL
                0x73,  // POWERLEVEL
                0x70,  // CONFIGURATION
                0x86,  // VERSION
                0x72   // MANUFACTURER_SPECIFIC
            };
            Msg msg( zdef::Type::Request, get_id() );
            msg.append( std::move( setdataFrame ) );
            getCommandInvoker().setTxdata( std::move( msg ) );
        }

        {
            std::vector<uint8_t> setdataFrame{ funcID, nodeStatus::PROTOCOL_DONE, bSource, 00 };
            Msg msg( zdef::Type::Request, get_id() );
            msg.append( std::move( setdataFrame ) );
            getCommandInvoker().setTxdata( std::move( msg ) );
        }

        break;
    }
    case addMode::NODE_STOP: {
        {
            uint8_t nodeId = device.getNodeId();
            std::vector<uint8_t> setdataFrame{ funcID, nodeStatus::DONE, nodeId, 0x00 };
            Msg msg( zdef::Type::Request, get_id() );
            msg.append( std::move( setdataFrame ) );
            getCommandInvoker().setTxdata( std::move( msg ) );
        }

        break;
    }
    }
}
