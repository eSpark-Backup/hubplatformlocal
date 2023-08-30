#ifndef DEVICE_H_
#define DEVICE_H_

#include <map>
#include <memory>

#include "cc/CommanClass.h"
#include "Zwavenode.h"

class Device : public Zwavenode {
public:
    Device( std::uint32_t homeId, uint8_t nodeId )
    : Zwavenode( homeId, nodeId ){};
    ~Device() = default;
    void inclusionSetReady();
    void inclusionClearReady();
    bool inclusionIsReady();

    void addCC( std::initializer_list<std::shared_ptr<CommandClass>> CC );
    std::shared_ptr<CommandClass> getCC( uint8_t n );
    uint8_t getNumCC();

private:
    bool m_inclusionReady;
    std::map<uint8_t, std::shared_ptr<CommandClass>> m_CC;
};

#endif /* DEVICE_H_ */
