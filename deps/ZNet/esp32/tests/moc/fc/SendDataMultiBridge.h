#ifndef SENDDATAMULTIBRIDGE_H_
#define SENDDATAMULTIBRIDGE_H_

#include <vector>
#include <cstdint>
#include "fc/Command.h"

class SendDataMultiBridge : public Command {
public:
    SendDataMultiBridge( ZwaveHub& hub, std::vector<uint8_t> const& pack );
    SendDataMultiBridge() = delete;
    virtual ~SendDataMultiBridge() = default;
    virtual void execute() override;

private:
    std::vector<uint8_t> const& m_pack;
};

#endif /* SENDDATAMULTIBRIDGE_H_ */
