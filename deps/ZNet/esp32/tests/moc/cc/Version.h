#ifndef BASIC_H_
#define BASIC_H_

#include <cstdint>

#include "../cc/CommanClass.h"

class Version : public CommandClass {
public:
    enum Command { get = 0x11, report = 0x12 };

    Version( uint8_t ZWaveLibraryType = 1,
             uint8_t ZWaveProtocolVersion = 1,
             uint8_t ZWaveProtocolSubVersion = 0,
             uint8_t ApplicationVersion = 1,
             uint8_t ApplicationSubVersion = 0 );
    virtual ~Version() = default;
    bool run( std::vector<std::vector<uint8_t>>& rev,
              std::vector<uint8_t>& ret,
              std::vector<uint8_t> const& data ) override;

private:
    uint8_t m_ZWaveLibraryType;
    uint8_t m_ZWaveProtocolVersion;
    uint8_t m_ZWaveProtocolSubVersion;
    uint8_t m_ApplicationVersion;
    uint8_t m_ApplicationSubVersion;
};

#endif /* BASIC_H_ */
