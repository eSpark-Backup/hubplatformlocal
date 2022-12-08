#pragma once

namespace znet {

enum class BarrierOperatorSignal;

namespace barrier_operator_signal_converter {

const char* signalToString( BarrierOperatorSignal id ) noexcept;

BarrierOperatorSignal stringToSignal( const char* name ) noexcept;

}  // namespace barrier_operator_signal_converter
}  // namespace znet
