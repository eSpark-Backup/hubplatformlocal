#include <znet/converters/barrier_operator_state_converter.hpp>

#include <znet/enums/barrier_operator_state.hpp>
#include <znet/converters/resources.hpp>
#include <znet/znet_utils.hpp>

extern "C" {
#include <znet_log.h>
}

namespace znet {
namespace barrier_operator_state_converter {

using namespace resources::cc_barrier_operator_state;

namespace {

struct BarrierOperatorStateData {
    const BarrierOperatorState scale;
    const char* const name;
};

const BarrierOperatorStateData barrierOperatorStateData[] = {
    { BarrierOperatorState::Closed, type::Closed },   { BarrierOperatorState::Closing, type::Closing },
    { BarrierOperatorState::Stopped, type::Stopped }, { BarrierOperatorState::Opening, type::Opening },
    { BarrierOperatorState::Opened, type::Opened },
};

const BarrierOperatorStateData* findBarrierOperatorStateData( BarrierOperatorState scale ) {
    for( const auto& data : barrierOperatorStateData ) {
        if( data.scale == scale ) {
            return &data;
        }
    }

    ZNET_LOGE( "Unsupported BarrierOperatorState\n\n" );

    return nullptr;
}

const BarrierOperatorStateData* findBarrierOperatorStateData( const char* name ) {
    for( const auto& data : barrierOperatorStateData ) {
        if( strcmp( data.name, name ) == 0 ) {
            return &data;
        }
    }

    ZNET_LOGE( "Unsupported BarrierOperatorState\n\n" );

    return nullptr;
}

}  // namespace

const char* typeToString( BarrierOperatorState scale ) noexcept {
    auto date = findBarrierOperatorStateData( scale );
    return date ? date->name : nullptr;
}

BarrierOperatorState stringToType( const char* name ) noexcept {
    auto date = findBarrierOperatorStateData( name );
    return date ? date->scale : BarrierOperatorState::Unknown;
}

}  // namespace barrier_operator_state_converter
}  // namespace znet
