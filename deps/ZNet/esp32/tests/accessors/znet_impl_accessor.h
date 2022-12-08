#pragma once

#include <znet_impl.hpp>

namespace znet {
namespace ut {

class ZNetImplAccessor {
public:
    InterviewHandler& getInterviewHandler( ZNetImpl& impl );

    CurrentValuesExtendedHandler& getCurrentValuesExtendedHandler( ZNetImpl& impl );

    CurrentValuesHandler& getCurrentValuesHandler( ZNetImpl& impl );

    znet_callbacks_t& getZnetCallbacks( ZNetImpl& impl );
};

}  // namespace ut
}  // namespace znet
