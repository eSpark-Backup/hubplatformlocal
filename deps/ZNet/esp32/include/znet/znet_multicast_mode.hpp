#pragma once

#include <znet/optional.hpp>

#include <cstdint>

namespace znet {

enum class MulticastMode : uint8_t {
    NoValuesGetting,
    ChipSinglecasts,
    SequentSinglecasts,

    Default = ChipSinglecasts
};

using MulticastModeOpt = utils::optional<MulticastMode>;

const char* multicastModeToString( MulticastMode mode );

MulticastModeOpt stringToMulticastMode( const char* str );

}  // namespace znet
