#ifndef TYPELIBRARY_H_
#define TYPELIBRARY_H_

#include "fc/Command.h"
#include <cstdint>
#include "Defs.h"

class TypeLibrary : public Command {
public:
    TypeLibrary( ZwaveHub& hub, std::vector<uint8_t> const& pack );
    virtual ~TypeLibrary() = default;
    virtual void execute() override;

private:
    std::vector<uint8_t> const& m_pack;
};

#endif /* TYPELIBRARY_H_ */
