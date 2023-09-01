#ifndef COMMANCLASS_H_
#define COMMANCLASS_H_

#include <cstdint>
#include <vector>

class CommandClass {
public:
    CommandClass( uint8_t id, uint8_t v );
    virtual ~CommandClass() = default;
    virtual bool run( std::vector<std::vector<uint8_t>>& rev,
                      std::vector<uint8_t>& ret,
                      std::vector<uint8_t> const& data ) = 0;
    uint8_t get_id();
    uint8_t get_version();

protected:
    enum CommandClassId {
        basic = 0x20,
        binarySwitch = 0x25,
        transportService = 0x55,
        version = 0x86,
        configuration = 0x70,
        manufacturerSpecific = 0x72,
        association = 0x85,
        security2 = 0x9F
    };

private:
    uint8_t m_id;
    uint8_t m_version;
};

#endif /* COMMANCLASS_H_ */
