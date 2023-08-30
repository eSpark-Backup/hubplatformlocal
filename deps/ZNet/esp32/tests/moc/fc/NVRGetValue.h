#ifndef NVRGETVALUE_H_
#define NVRGETVALUE_H_

#include <vector>
#include <cstdint>
#include "fc/Command.h"

class NVRGetValue : public Command {
public:
    NVRGetValue( ZwaveHub& hub, std::vector<uint8_t> const& pack );
    NVRGetValue() = delete;
    virtual ~NVRGetValue() = default;
    virtual void execute() override;

private:
    std::vector<uint8_t> const& m_pack;
};

#endif /* NVRGETVALUE_H_ */
