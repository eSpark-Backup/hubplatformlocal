#ifndef GETNODEPROTOCOLINFO_H_
#define GETNODEPROTOCOLINFO_H_

#include <vector>
#include <cstdint>
#include "fc/Command.h"

class GetNodeProtocolInfo : public Command {
public:
    GetNodeProtocolInfo( ZwaveHub& hub, std::vector<uint8_t> const& pack );
    GetNodeProtocolInfo() = delete;
    virtual ~GetNodeProtocolInfo() = default;
    virtual void execute() override;

private:
    std::vector<uint8_t> const& m_pack;
};

#endif /* GETNODEPROTOCOLINFO_H_ */
