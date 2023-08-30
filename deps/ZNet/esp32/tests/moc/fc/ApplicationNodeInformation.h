#ifndef APPLICATIONNODEINFORMATION_H_
#define APPLICATIONNODEINFORMATION_H_

#include "fc/Command.h"

class ApplicationNodeInformation : public Command {
public:
    ApplicationNodeInformation( ZwaveHub& hub );
    virtual ~ApplicationNodeInformation() = default;

    virtual void execute() override;
};

#endif /* APPLICATIONNODEINFORMATION_H_ */
