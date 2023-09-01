#ifndef MSG_H_
#define MSG_H_

#include <cstdint>
#include <cstddef>
#include <vector>
#include "Defs.h"

class Msg {
public:
    Msg( zdef::Type type, zdef::FunctionClassId fClassId );
    Msg() = delete;
    ~Msg() = default;
    void append( std::vector<uint8_t>&& data );
    void append( uint8_t data );
    void finalize();
    size_t getLength();
    bool isFinalize();
    std::vector<uint8_t>& getData();

private:
    std::vector<uint8_t> m_buffer;
    bool m_finalize = false;
};

#endif /* MSG_H_ */
