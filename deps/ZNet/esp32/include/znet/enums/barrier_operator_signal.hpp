#pragma once

namespace znet {

enum class BarrierOperatorSignal {
    AudibleNotification = 0x01,
    VisualNotification = 0x02,

    Unknown = 0x00
};

}  // namespace znet
