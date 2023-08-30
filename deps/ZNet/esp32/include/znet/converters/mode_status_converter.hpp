#pragma once

#include "znet/iznet.hpp"

namespace znet {

enum class ModeStatus;

namespace mode_status_converter {

const char* toString( ModeStatus modeStatus ) noexcept;

}  // namespace mode_status_converter
}  // namespace znet
