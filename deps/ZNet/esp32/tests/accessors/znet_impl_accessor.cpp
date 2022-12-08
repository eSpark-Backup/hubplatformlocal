#include "znet_impl_accessor.h"

namespace znet {
namespace ut {

InterviewHandler& ZNetImplAccessor::getInterviewHandler( ZNetImpl& impl ) {
    return impl.m_interviewHandler;
}

CurrentValuesExtendedHandler& ZNetImplAccessor::getCurrentValuesExtendedHandler( ZNetImpl& impl ) {
    return impl.m_currentValuesExtendedHandler;
}

CurrentValuesHandler& ZNetImplAccessor::getCurrentValuesHandler( ZNetImpl& impl ) {
    return impl.m_currentValuesHandler;
}

znet_callbacks_t& ZNetImplAccessor::getZnetCallbacks( ZNetImpl& impl ) {
    return impl.m_znet_cb;
}

}  // namespace ut
}  // namespace znet
