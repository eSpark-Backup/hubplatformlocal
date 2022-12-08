#ifndef DEFAULTCOMMANDHOLDER_H_
#define DEFAULTCOMMANDHOLDER_H_

#include <iostream>
#include <cstdint>
#include "fc/Command.h"

class DefaultCommandHolder : public Command {
public:
    DefaultCommandHolder( ZwaveHub& hub, uint8_t cmdid );
    virtual ~DefaultCommandHolder() = default;

    virtual void execute() override;

private:
    uint8_t m_commandId;
};

#endif /* DEFAULTCOMMANDHOLDER_H_ */
