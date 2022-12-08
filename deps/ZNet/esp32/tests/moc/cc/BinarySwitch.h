#ifndef BINARYSWITCH_H_
#define BINARYSWITCH_H_

#include <cstdint>

#include "../cc/CommanClass.h"

class BinarySwitch : public CommandClass {
public:
    BinarySwitch();
    virtual ~BinarySwitch() = default;
    bool run( std::vector<std::vector<uint8_t>>& rev,
              std::vector<uint8_t>& ret,
              std::vector<uint8_t> const& data ) override;

private:
    enum Command { set = 0x01, get = 0x02, report = 0x03 };

private:
    bool m_val;
};

#endif /* BINARYSWITCH_H_ */
