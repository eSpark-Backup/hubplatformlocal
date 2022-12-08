#ifndef ZWAVEERRORS_H_
#define ZWAVEERRORS_H_

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <cstdarg>

class ZwaveErrors {
public:
    static void error( bool expr, const char* format, ... ) {
        if( expr == false ) {
            va_list va;
            va_start( va, format );

            vfprintf( stderr, format, va );

            va_end( va );

            exit( EXIT_FAILURE );
        }
    }
};

#endif /* ZWAVEERRORS_H_ */
