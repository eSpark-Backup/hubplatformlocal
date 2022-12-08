#ifndef APPLICATIONCOMMANDHANDLER_H_
#define APPLICATIONCOMMANDHANDLER_H_

#include <vector>
#include <cstdint>
#include "Msg.h"

class ApplicationCommandHandler {
public:
    ApplicationCommandHandler( uint8_t sourceNode );
    ApplicationCommandHandler() = delete;
    ~ApplicationCommandHandler() = default;
    bool get( std::vector<Msg>& ret, std::vector<uint8_t>&& cmd );

private:
    // TODO: make set/get functions
    uint8_t m_rxStatus = 0x00;
    uint8_t m_sourceNode;
    uint8_t m_rxRSSIVal = 0xb9;
    uint8_t m_securityKey = 0x00;
};

#endif /* APPLICATIONCOMMANDHANDLER_H_ */
