#pragma once

#include <stdio.h>

class IFile {
public:
    virtual ~IFile() = default;

    virtual bool write( const void* src, size_t len ) = 0;
    virtual bool read( void* dst, size_t len ) = 0;
};
