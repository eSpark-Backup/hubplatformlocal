#ifndef ASSOCIATION_H_
#define ASSOCIATION_H_

#include <map>
#include <vector>
#include "cc/CommanClass.h"

class Association : public CommandClass {
public:
    Association();
    virtual ~Association() = default;
    bool run( std::vector<std::vector<uint8_t>>& rev,
              std::vector<uint8_t>& ret,
              std::vector<uint8_t> const& data ) override;

private:
    enum Command { set = 0x01, get = 0x02, report = 0x03, remove = 0x04 };

private:
    bool cset( std::vector<uint8_t>& ret, std::vector<uint8_t> const& data );
    bool cget( std::vector<uint8_t>& ret, std::vector<uint8_t> const& data );

private:
    uint8_t m_maxNodesSupported = 10;
    std::map<uint8_t, std::vector<uint8_t>> m_group;
};

#endif /* ASSOCIATION_H_ */
