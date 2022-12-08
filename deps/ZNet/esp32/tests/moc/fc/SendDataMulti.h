#ifndef SENDDATAMULTI_H_
#define SENDDATAMULTI_H_

#include <vector>
#include <cstdint>
#include "fc/Command.h"

class SendDataMulti : public Command {
public:
    SendDataMulti( ZwaveHub& hub, std::vector<uint8_t> const& pack );
    SendDataMulti() = delete;
    virtual ~SendDataMulti() = default;
    virtual void execute() override;

private:
    std::vector<uint8_t> const& m_pack;
};

#endif /* SENDDATAMULTI_H_ */
