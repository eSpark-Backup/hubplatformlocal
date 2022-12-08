#include "Command.h"

Command::Command( zdef::FunctionClassId id, ZwaveHub& hub )
: m_invoker{ hub }
, m_id( id ){};

zdef::FunctionClassId Command::Command::get_id() {
    return m_id;
}
