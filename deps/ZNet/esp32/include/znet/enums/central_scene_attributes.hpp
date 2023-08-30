#pragma once

namespace znet {

enum class CentralSceneAttributes {
    Press1Time = 0x00,
    Released = 0x01,
    HeldDown = 0x02,
    Press2Times = 0x03,
    Press3Times = 0x04,
    Press4Times = 0x05,
    Press5Times = 0x06,

    Start = Press1Time,
    End = Press5Times,
};

}  // namespace znet
