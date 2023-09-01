#include "znet_handler_values_extended.hpp"

#include "znet_impl.hpp"
#include "znet/cc_0x4e_schedule_entry_lock.hpp"
#include "znet/cc_0x63_user_code.hpp"

extern "C" {
#include "znet_log.h"
}

namespace znet {

CurrentValuesExtendedHandler::CurrentValuesExtendedHandler( ZNetImpl& parent )
: CurrentValuesHandler( parent ) {
    pushToRequestOrder( ZNET_COMMAND_CLASS_USER_CODE );
    pushToRequestOrder( ZNET_COMMAND_CLASS_SCHEDULE_ENTRY_LOCK );
}

}  // namespace znet
