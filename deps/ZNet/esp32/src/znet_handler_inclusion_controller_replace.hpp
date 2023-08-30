#pragma once

#include <znet_handler_inclusion_controller.hpp>

namespace znet {

class InclusionReplaceHandler : public InclusionHandler {
public:
    InclusionReplaceHandler( ZNetImpl& parent );
};

}  // namespace znet
