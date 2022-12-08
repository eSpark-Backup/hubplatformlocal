#ifndef SETSUCNODEID_H_
#define SETSUCNODEID_H_

#include "fc/Command.h"
#include <cstdint>
#include "Defs.h"

class SetSUCNodeID : public Command {
public:
    SetSUCNodeID( ZwaveHub& hub, std::vector<uint8_t> const& pack );
    virtual ~SetSUCNodeID() = default;
    virtual void execute() override;

private:
    std::vector<uint8_t> const& m_pack;
};

#endif /* SETSUCNODEID_H_ */
