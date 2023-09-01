#ifndef SENDDATA_H_
#define SENDDATA_H_

#include <vector>
#include <cstdint>
#include "fc/Command.h"

class SendData : public Command {
public:
    SendData( ZwaveHub& hub, std::vector<uint8_t> const& pack );
    SendData() = delete;
    virtual ~SendData() = default;
    virtual void execute() override;

private:
    std::vector<uint8_t> const& m_pack;
};

#endif /* SENDDATA_H_ */
