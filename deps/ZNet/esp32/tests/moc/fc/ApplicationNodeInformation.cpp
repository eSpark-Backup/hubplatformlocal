#include <iostream>
#include "ApplicationNodeInformation.h"
#include "Defs.h"

ApplicationNodeInformation::ApplicationNodeInformation( ZwaveHub& hub )
: Command( zdef::FunctionClassId::ApplicationNodeInformation, hub ){};

void ApplicationNodeInformation::execute() {
    std::cout << "ApplicationNodeInformation" << std::endl;
    getCommandInvoker().setAck();
}
