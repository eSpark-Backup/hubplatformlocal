#ifndef TRANSPORTSERVICE_H_
#define TRANSPORTSERVICE_H_

#include <vector>

#include "../cc/CommanClass.h"

class TransportService : public CommandClass {
public:
    TransportService();
    virtual ~TransportService() = default;
    bool run( std::vector<std::vector<uint8_t>>& rev,
              std::vector<uint8_t>& ret,
              std::vector<uint8_t> const& data ) override;

private:
    std::uint16_t tcrc16( const uint8_t* bytes, int length );

private:
    enum packType { start = 0xC0, subsequent = 0xE0, end = 0xE8 };

private:
    std::vector<uint8_t> m_datapack;
};

#endif /* TRANSPORTSERVICE_H_ */
