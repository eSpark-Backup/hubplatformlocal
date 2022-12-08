#include "DataFrame.h"

DataFrame DataFrameGen::toDataFrame( std::vector<uint8_t> const& data ) {
    // TODO Check Checksum

    DataFrame dataFrame;

    dataFrame.m_sof = data[0];
    dataFrame.m_packLength = data[1];
    dataFrame.m_type = static_cast<zdef::Type>( data[2] );
    dataFrame.m_commandId = data[3];

    for( int i = 0; i < dataFrame.m_packLength - 1 /*Length*/ - 1 /*Type*/ - 1 /*sApiCmd*/; i++ ) {
        dataFrame.m_parameterPack.push_back( data[i + 1 /*SOF*/ + 1 /*Length*/ + 1 /*Type*/ + 1 /*sApiCmd*/] );
    }

    uint8_t crc = data[dataFrame.m_packLength - 1];

    return dataFrame;
}
