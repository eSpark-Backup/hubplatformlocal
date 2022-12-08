#ifndef DATAFRAME_H_
#define DATAFRAME_H_

#include <string>
#include <cstdint>
#include <vector>
#include "Defs.h"

struct DataFrame {
    uint8_t m_sof;
    uint8_t m_packLength;
    zdef::Type m_type;
    uint8_t m_commandId;
    std::vector<uint8_t> m_parameterPack;
    uint8_t m_crc;
};

class DataFrameGen {
public:
    static DataFrame toDataFrame( std::vector<uint8_t> const& data );
};

#endif /* DATAFRAME_H_ */
