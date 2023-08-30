#ifndef SENDDATABRIDGE_H_
#define SENDDATABRIDGE_H_

#include <vector>
#include <cstdint>
#include "fc/Command.h"

class SendDataBridge : public Command {
public:
    SendDataBridge( ZwaveHub& hub, std::vector<uint8_t> const& pack );
    SendDataBridge() = delete;
    virtual ~SendDataBridge() = default;
    virtual void execute() override;

private:
    std::vector<uint8_t> const& m_pack;
};

#endif /* SENDDATABRIDGE_H_ */
