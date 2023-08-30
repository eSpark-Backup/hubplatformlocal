#ifndef STORAGE_H_
#define STORAGE_H_

#include <stdint.h>
#include <stddef.h>
#include <vector>
#include <cstdint>

class Storage {
public:
    enum class Version { v0_1 };
    Storage( Version version );
    ~Storage() = default;
    static int save( size_t offset, const void* data, size_t size, void* arg );
    static int load( size_t offset, void* data, size_t size, void* arg );
    static int reset( void* arg );

private:
    Version m_version;
    static const std::size_t m_store_size = 16 * 1024;
    std::vector<uint8_t> m_data;
};

#endif /* STORAGE_H_ */
