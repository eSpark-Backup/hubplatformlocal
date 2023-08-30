#pragma once

#include <cstdlib>
#include <string>
#include <vector>
#include <znet/optional.hpp>

#include <znet/znet.h>

namespace znet {

using ZwaveAllianceLockingId = uint16_t;
using ZwaveAllianceLockingIdOpt = utils::optional<ZwaveAllianceLockingId>;

struct AntiTheftInfo;
using AntiTheftInfoOpt = utils::optional<AntiTheftInfo>;

struct AntiTheftInfo {
    AntiTheftInfo() = default;

    AntiTheftInfo( bool isRestricted,
                   std::vector<uint8_t> hint,
                   uint16_t manufacturerId,
                   ZwaveAllianceLockingIdOpt lockingId );

    bool operator==( const AntiTheftInfo& other ) const;

    static AntiTheftInfoOpt makeFromReport( const znet_cmd_anti_theft_report_t& antiTheftReport );

    std::string getHexHint() const;

    const char* getManufacturerName() const;

    bool m_isRestricted = false;
    std::vector<uint8_t> m_hint;
    uint16_t m_manufacturerId = 0;
    ZwaveAllianceLockingIdOpt m_zwaveAllianceLockingId;
};

}  // namespace znet
