#ifndef ZWAVENODE_H_
#define ZWAVENODE_H_

#include <cstdint>

class Zwavenode {
public:
    Zwavenode( std::uint32_t homeId, uint8_t nodeId );
    Zwavenode() = delete;
    virtual ~Zwavenode(){};
    std::uint32_t getHomeId() const;
    uint8_t getNodeId() const;

private:
    std::uint32_t m_homeId;
    uint8_t m_nodeId;
};

inline std::uint32_t Zwavenode::getHomeId() const {
    return m_homeId;
}

inline uint8_t Zwavenode::getNodeId() const {
    return m_nodeId;
}

#endif /* ZWAVENODE_H_ */
