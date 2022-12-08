#pragma once

#include <chrono>
#include <tuple>
#include <functional>

namespace znet {

template <typename... Args>
struct RequestParams {};

template <typename... Args>
struct RespondParams {};

template <typename...>
struct RequestInfo;

template <typename... RequestArgs, typename RequestDoer, typename... RespondArgs>
struct RequestInfo<RequestParams<RequestArgs...>, RequestDoer, RespondParams<RespondArgs...>>
: std::tuple<RequestArgs...> {
    using Respond = std::tuple<RespondArgs...>;
    using Data = std::tuple<RequestArgs...>;
    using RespondCallback = std::function<void( RespondArgs... )>;
    using Data::Data;
    using Doer = RequestDoer;
};

static constexpr size_t MaxRetryRequestNumber = 4;
static constexpr size_t MaxFailNumber = 2;
static constexpr std::chrono::seconds RequestCheckingPeriod{ 3 };
static constexpr std::chrono::seconds RequestSendingPeriod{ 21 };

}  // namespace znet
