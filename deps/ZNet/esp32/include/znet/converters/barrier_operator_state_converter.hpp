#pragma once

namespace znet {

enum class BarrierOperatorState;

namespace barrier_operator_state_converter {

const char* typeToString( BarrierOperatorState scale ) noexcept;

BarrierOperatorState stringToType( const char* name ) noexcept;

}  // namespace barrier_operator_state_converter
}  // namespace znet
