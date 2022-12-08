#pragma once

#include <znet_handler_values.hpp>

namespace znet {

class CurrentValuesExtendedHandler : public CurrentValuesHandler {
public:
    CurrentValuesExtendedHandler( ZNetImpl& parent );

    ~CurrentValuesExtendedHandler() = default;
};

}  // namespace znet
