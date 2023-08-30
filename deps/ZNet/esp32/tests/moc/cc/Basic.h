#ifndef BASIC_H_
#define BASIC_H_

#include <cstdint>

#include "../cc/CommanClass.h"

class Basic : public CommandClass {
public:
    Basic();
    virtual ~Basic() = default;
    bool run( std::vector<std::vector<uint8_t>>& rev,
              std::vector<uint8_t>& ret,
              std::vector<uint8_t> const& data ) override;

private:
    enum Command { set = 0x01, get = 0x02, report = 0x03 };

private:
    uint8_t m_val;
};

#endif /* BASIC_H_ */
