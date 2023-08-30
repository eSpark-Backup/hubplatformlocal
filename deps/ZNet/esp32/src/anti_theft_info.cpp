#include "znet/anti_theft_info.hpp"
#include "znet/znet_utils.hpp"
#include "znet/converters/manufacturer_names_converter.hpp"

namespace znet {

AntiTheftInfo::AntiTheftInfo( bool isRestricted,
                              std::vector<uint8_t> hint,
                              uint16_t manufacturerId,
                              ZwaveAllianceLockingIdOpt lockingId )
: m_isRestricted( isRestricted )
, m_hint( std::move( hint ) )
, m_manufacturerId( manufacturerId )
, m_zwaveAllianceLockingId( lockingId ) {}

AntiTheftInfoOpt AntiTheftInfo::makeFromReport( const znet_cmd_anti_theft_report_t& antiTheftReport ) {
    return antiTheftReport.is_locked ?
               AntiTheftInfo(
                   antiTheftReport.is_restricted,
                   std::vector<uint8_t>{ antiTheftReport.hint, antiTheftReport.hint + antiTheftReport.hint_length },
                   antiTheftReport.manufacturer_id,
                   antiTheftReport.zwave_locking_id ? antiTheftReport.zwave_locking_id : ZwaveAllianceLockingIdOpt() ) :
               AntiTheftInfoOpt();
}

bool AntiTheftInfo::operator==( const AntiTheftInfo& other ) const {
    return m_isRestricted == other.m_isRestricted && m_hint == other.m_hint &&
           m_manufacturerId == other.m_manufacturerId && m_zwaveAllianceLockingId == other.m_zwaveAllianceLockingId;
}

std::string AntiTheftInfo::getHexHint() const {
    return utils::bytesToHex( m_hint );
}

const char* AntiTheftInfo::getManufacturerName() const {
    return manufacturer_names_converter::manufacturerIdToName( m_manufacturerId );
}

}  // namespace znet
