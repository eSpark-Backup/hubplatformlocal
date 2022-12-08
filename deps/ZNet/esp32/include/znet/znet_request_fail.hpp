#pragma once

#include <utility>
#include <znet/optional.hpp>

namespace znet {

enum class RequestFailType : std::uint8_t { Timeout, ReportedFail, SendingError };

enum class RequestFailHandlingResult : uint8_t { RequestContinued, RequestStopped, RequestNotFound };

using RequestFailTypeOpt = utils::optional<RequestFailType>;

}  // namespace znet
