#include "../cc/ManufacturerSpecific.h"

#include <assert.h>

ManufacturerSpecific::ManufacturerSpecific( idStruct_type idStruct )
: CommandClass( CommandClass::CommandClassId::manufacturerSpecific, 1 )
, m_idStruct( idStruct ) {}

bool ManufacturerSpecific::run( std::vector<std::vector<uint8_t>>& rev,
                                std::vector<uint8_t>& ret,
                                std::vector<uint8_t> const& data ) {
    switch( data.at( 1 ) ) {
    case Command::get: {
        uint8_t manufacturerId1 = m_idStruct.manufacturerId >> 8;
        uint8_t manufacturerId2 = m_idStruct.manufacturerId & 0xFF;
        uint8_t productTypeId1 = m_idStruct.productTypeId >> 8;
        uint8_t productTypeId2 = m_idStruct.productTypeId & 0xFF;
        uint8_t productId1 = m_idStruct.productId >> 8;
        uint8_t productId2 = m_idStruct.productId & 0xFF;
        ret = { get_id(),       Command::report, manufacturerId1, manufacturerId2,
                productTypeId1, productTypeId2,  productId1,      productId2 };
        break;
    }
        /*case Command::report:
            break;*/

    default:
        assert( !"Error Command" );
    }

    return true;
}
