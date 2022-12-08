#pragma once

namespace znet {

enum class BarrierOperatorState {
    Closed = 0x00,
    Closing = 0xFC,
    Stopped = 0xFD,
    Opening = 0xFE,
    Opened = 0xFF,

    Unknown = 0x01
};

}  // namespace znet
