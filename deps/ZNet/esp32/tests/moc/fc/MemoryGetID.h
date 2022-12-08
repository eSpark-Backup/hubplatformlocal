#ifndef MEMORYGETID_H_
#define MEMORYGETID_H_

#include "fc/Command.h"

class MemoryGetID : public Command {
public:
    MemoryGetID( ZwaveHub& hub );
    virtual ~MemoryGetID() = default;
    virtual void execute() override;
};

#endif /* SERIALAPICOMMANDS_MEMORYGETID_H_ */
