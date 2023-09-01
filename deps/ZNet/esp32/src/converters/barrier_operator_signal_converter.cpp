#include <znet/converters/barrier_operator_signal_converter.hpp>

#include <znet/enums/barrier_operator_signal.hpp>
#include <znet/converters/resources.hpp>
#include <znet/znet_utils.hpp>

extern "C" {
#include <znet_log.h>
}

namespace znet {
namespace barrier_operator_signal_converter {

using namespace resources::cc_barrier_operator_signal;

namespace {

struct BarrierOperatorSignalData {
    const BarrierOperatorSignal id;
    const char* const name;
};

const BarrierOperatorSignalData barrierOperatorSignalData[] = {
    { BarrierOperatorSignal::AudibleNotification, AudibleNotification },
    { BarrierOperatorSignal::VisualNotification, VisualNotification }
};

const BarrierOperatorSignalData* findBarrierOperatorSignalData( BarrierOperatorSignal id ) {
    for( const auto& data : barrierOperatorSignalData ) {
        if( data.id == id ) {
            return &data;
        }
    }

    ZNET_LOGE( "Unsupported BarrierOperatorSignal\n\n" );

    return nullptr;
}

const BarrierOperatorSignalData* findBarrierOperatorSignalData( const char* name ) {
    for( const auto& data : barrierOperatorSignalData ) {
        if( strcmp( data.name, name ) == 0 ) {
            return &data;
        }
    }

    ZNET_LOGE( "Unsupported BarrierOperatorSignal name\n\n" );

    return nullptr;
}

}  // namespace

const char* signalToString( BarrierOperatorSignal id ) noexcept {
    auto date = findBarrierOperatorSignalData( id );
    return date ? date->name : nullptr;
}

BarrierOperatorSignal stringToSignal( const char* name ) noexcept {
    auto date = findBarrierOperatorSignalData( name );
    return date ? date->id : BarrierOperatorSignal::Unknown;
}

}  // namespace barrier_operator_signal_converter
}  // namespace znet
