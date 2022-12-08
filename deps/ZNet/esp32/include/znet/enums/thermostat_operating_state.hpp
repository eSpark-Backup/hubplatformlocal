#pragma once

namespace znet {

enum class ThermostatOperatingState {
    Idle,
    Heating,
    Cooling,
    FanOnly,
    PendingHeat,
    PendingCool,
    VentEco,
    AuxHeating,
    SecStageHeating,
    SecStageCooling,
    SecStageAuxHeat,
    ThirdStageAuxHeat,

    Count
};

}  // namespace znet
