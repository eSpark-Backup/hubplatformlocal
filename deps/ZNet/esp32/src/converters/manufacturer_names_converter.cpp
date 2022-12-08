#include <znet/converters/manufacturer_names_converter.hpp>

#include <algorithm>
#include <cassert>
#include <iterator>

namespace znet {
namespace manufacturer_names_converter {

struct __attribute__( ( packed ) ) ManufacturerInfo {
    const uint16_t m_id;
    const char* m_name;

    bool operator<( const ManufacturerInfo& rhs ) const {
        return m_id < rhs.m_id;
    }
};
const ManufacturerInfo names[] = { { 1, "Advanced Control Technologies" },
                                   { 2, "Danfoss" },
                                   { 5, "Intermatic" },
                                   { 12, "HomeSeer" },
                                   { 16, "NuTone" },
                                   { 26, "Eaton" },
                                   { 29, "Leviton" },
                                   { 30, "Express Controls" },
                                   { 51, "Electronic Solutions" },
                                   { 57, "Honeywell" },
                                   { 59, "Schlage" },
                                   { 71, "Somfy" },
                                   { 89, "Secure" },
                                   { 96, "Everspring" },
                                   { 99, "GE/Jasco" },
                                   { 126, "Monster Cable" },
                                   { 132, "Fortrezz" },
                                   { 134, "Aeotec" },
                                   { 138, "Wintop" },
                                   { 139, "Trane" },
                                   { 144, "Kwikset" },
                                   { 151, "Wintop" },
                                   { 152, "RT" },
                                   { 265, "Vision" },
                                   { 270, "Poly-Control" },
                                   { 271, "Fibaro" },
                                   { 275, "Evolve" },
                                   { 276, "Kichler" },
                                   { 277, "Z-Wave.Me" },
                                   { 278, "Everspring" },
                                   { 282, "Enerwave Green Automation" },
                                   { 287, "Ecolink" },
                                   { 297, "Yale" },
                                   { 305, "Zipato" },
                                   { 306, "DynaQuip Controls" },
                                   { 312, "First Alert" },
                                   { 313, "Aizyc" },
                                   { 316, "Philio" },
                                   { 330, "Ecolink" },
                                   { 335, "NuTone" },
                                   { 337, "Sercomm" },
                                   { 340, "Popp" },
                                   { 343, "EcoNet Controls" },
                                   { 345, "Qubino" },
                                   { 351, "Horus" },
                                   { 357, "NodOn" },
                                   { 362, "Aeon" },
                                   { 376, "Nexia" },
                                   { 382, "Telguard" },
                                   { 398, "Climax" },
                                   { 400, "Building 36" },
                                   { 410, "Sensative" },
                                   { 520, "Hank" },
                                   { 526, "Domitech" },
                                   { 535, "Westinghouse" },
                                   { 543, "Dome" },
                                   { 560, "ID Lock" },
                                   { 569, "Stelpro" },
                                   { 590, "Digital Home System" },
                                   { 600, "NEO Coolcam" },
                                   { 603, "Ezlo" },
                                   { 608, "Heiman" },
                                   { 619, "Ei Electronics" },
                                   { 622, "Springs Window Fashions" },
                                   { 634, "zooZ" },
                                   { 664, "Horus" },
                                   { 768, "Leedarson" },
                                   { 775, "SATCO" },
                                   { 789, "ZLINK" },
                                   { 831, "August" },
                                   { 837, "Swidget" },
                                   { 838, "Ring" },
                                   { 870, "Philia" },
                                   { 881, "Aeotec" },
                                   { 1032, "Gerber" },
                                   { 1037, "Delaney Hardware" },
                                   { 4231, "Dome" },
                                   { 21076, "Remotec" } };

const char* manufacturerIdToName( const uint16_t id ) {
    assert( std::is_sorted( std::begin( names ), std::end( names ) ) );

    auto findResult = std::lower_bound( std::begin( names ), std::end( names ), ManufacturerInfo{ id, "" } );
    return ( findResult != std::end( names ) && findResult->m_id == id ) ? findResult->m_name : "Unknown";
}

}  // namespace manufacturer_names_converter
}  // namespace znet
