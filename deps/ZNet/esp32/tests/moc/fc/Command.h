#ifndef COMMAND_H_
#define COMMAND_H_

#include "ZwaveHub.h"
#include "Defs.h"

class Command {
public:
    explicit Command( zdef::FunctionClassId id, ZwaveHub& hub );
    virtual ~Command() = default;
    virtual void execute() = 0;
    ZwaveHub& getCommandInvoker() {
        return m_invoker;
    }
    zdef::FunctionClassId get_id();

private:
    ZwaveHub& m_invoker;
    zdef::FunctionClassId m_id;
};

#endif /* COMMAND_H_ */
