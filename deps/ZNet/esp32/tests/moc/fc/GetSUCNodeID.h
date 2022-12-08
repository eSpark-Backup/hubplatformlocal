#ifndef GETSUCNODEID_H_
#define GETSUCNODEID_H_

#include "fc/Command.h"

class GetSUCNodeID : public Command {
public:
    GetSUCNodeID( ZwaveHub& hub );
    virtual ~GetSUCNodeID() = default;
    virtual void execute() override;
};

#endif /* GETSUCNODEID_H_ */
