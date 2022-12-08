#ifndef SETTHRESHOLD_H_
#define SETTHRESHOLD_H_

#include "fc/Command.h"
#include <cstdint>
#include "Defs.h"

class SetThreshold : public Command {
public:
    SetThreshold( ZwaveHub& hub, std::vector<uint8_t> const& pack );
    virtual ~SetThreshold() = default;
    virtual void execute() override;

private:
    std::vector<uint8_t> const& m_pack;
};

#endif /* SETTHRESHOLD_H_ */
