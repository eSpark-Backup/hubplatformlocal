#ifndef LIBRARYVERSION_H_
#define LIBRARYVERSION_H_

#include "fc/Command.h"
#include <cstdint>
#include "Defs.h"

class LibraryVersion : public Command {
public:
    LibraryVersion( ZwaveHub& hub, std::vector<uint8_t> const& pack );
    virtual ~LibraryVersion() = default;
    virtual void execute() override;

private:
    std::vector<uint8_t> const& m_pack;
};

#endif /* LIBRARYVERSION_H_ */
