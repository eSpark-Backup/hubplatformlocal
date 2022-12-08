#include "znet/znet_multicast_mode.hpp"

#include "znet/znet_utils.hpp"

namespace znet {

namespace {

using StringList = std::initializer_list<const char* const>;
using RangedModesList = utils::RangedList<MulticastMode, const char*>;

const StringList ModeNamesList_0 = {
    "no_values_getting",   // 0
    "chip_singlecasts",    // 1
    "sequent_singlecasts"  // 2
};
const RangedModesList ModeNamesList = { { MulticastMode::NoValuesGetting, ModeNamesList_0 } };

}  // namespace

const char* multicastModeToString( MulticastMode mode ) {
    const auto result = utils::findInRangedList( ModeNamesList, mode );
    return result ? *result : nullptr;
}

MulticastModeOpt stringToMulticastMode( const char* str ) {
    return utils::findIndexInRangedList( ModeNamesList,
                                         [str]( const char* modeName ) { return strcmp( modeName, str ) == 0; } );
}

}  // namespace znet
