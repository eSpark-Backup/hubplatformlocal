
#include <znet_handler_inclusion_controller_replace.hpp>
#include <znet_impl.hpp>
#include <znet/znet.h>

extern "C" {
#include <znet_log.h>
}

namespace znet {

InclusionReplaceHandler::InclusionReplaceHandler( ZNetImpl& parent )
: InclusionHandler( parent ) {}

}  // namespace znet
