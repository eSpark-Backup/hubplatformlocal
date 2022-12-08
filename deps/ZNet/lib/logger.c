//////////////////////////////////////////////////////////////////////////////

#include "logger.h"

//////////////////////////////////////////////////////////////////////////////

/*
//----------------------------------------------------------------------------
void
ICACHE_FLASH_ATTR
znet_log(const  ZWLogLevel level, const char* message, ...)
{
    if( znet == NULL ) return;

    if( level >= znet->_log_level ) {
        va_list arg;
        va_start(arg, message);

        LOCK_ENTER(&znet->_log_mutex);

        FILE *file = znet->_log_file; if( file == NULL ) file = stderr;

        struct tm t;
        int msec;

        get_local_time(&t, &msec);

        char timebuffer[30];
#ifdef _WINDOWS
        strftime(timebuffer, 30, "%Y-%m-%d %X", &t);
#else
        strftime(timebuffer, 30, "%Y-%m-%d %T", &t);
#endif
        fprintf(file, "[%s.%03d] ", timebuffer, msec);

        flockfile(file);
        vfprintf(file, message, arg);
        fflush(file);
        funlockfile(file);

        LOCK_EXIT(&znet->_log_mutex);

        va_end(arg);
    }
}
*/
/*
//----------------------------------------------------------------------------
void
ICACHE_FLASH_ATTR
znet_dump(const  ZWLogLevel level, const char* prefix, int length,
const uint8_t* buffer)
{
    if( znet == NULL ) return;

    if( level >= znet->_log_level ) {
        LOCK_ENTER(&znet->_log_mutex);

        FILE *file = znet->_log_file; if( file == NULL ) file = stderr;

        if( prefix && *prefix != '\0' ) fprintf(file, "%s", prefix);

        if( length == 0 ) {
            fprintf(file, "( zero-length buffer )\r\n");
        }
        else {
            fprintf(file, "( ");
            for( int i = 0; i < length; i++ ) {
                fprintf(file, "%02X ", buffer[i]);
            }
            fprintf(file, "\r\n");
        }

        LOCK_EXIT(&znet->_log_mutex);
    }
}
*/
/*
//----------------------------------------------------------------------------
const char*
ICACHE_FLASH_ATTR
znet_strerror(ZWError err)
{
        switch( err ) {
                case NoError         : return "No error";
                case InvalidArg      : return "Bad arguments";
                case BadAllocation   : return "Memory allocation error";
                case NotImplemented  : return "Not implemented by the library";
                case NotSupported    : return "Function class not supported by
the hardware"; case AccessDenied    : return "Access denieded"; case
ThreadingError  : return "Threading error"; case InvalidOperation: return
"Invalid operation"; case InternalError   : return "Internal consistency error";
                case BadData         : return "Wrong packet from Z-Wave network
or Discovery got bad data (stick communication failed)"; case InvalidType     :
return "Wrong data type requested"; case DuplicateObject : return "Queueing of
duplicate job. Current job removed. Not an error."; case InvalidThread   :
return "Object accessed from invalid thread"; case InvalidPort     : return "Can
not open port"; case InvalidConfig   : return "Error reading configuration and
transation files"; case NotPrimary      : return "Controller is not primary";
                default : return "No string description for this error";
        }
}
*/
/*
//----------------------------------------------------------------------------
void
ICACHE_FLASH_ATTR
znet_log_error(const  ZWLogLevel level, const char* message, ZWError
err)
{

    //znet_log(znet, level, "%s: %s (%d)\r\n", message, znet_strerror(err),
err);
}
*/
void print_hex_buf( const uint8_t* buf, int len ) {
    if( !buf ) {
        Log_Write( "<NULL> " );
        return;
    }
    int i;
    for( i = 0; i < len; i++ ) {
        Log_Write( "%02X ", buf[i] );
    }
    Log_Write( "\n" );
}

#ifdef DEBUG

/*
//----------------------------------------------------------------------------
void
ICACHE_FLASH_ATTR
znet_debug_log_error(const  ZWError err, const char* func, const
char* file, int line)
{
    //if( err != NoError )znet_log(znet, Error, "Error returned from %s at
%s:%d: %s (%d)\r\n", func, file, line, znet_strerror(err), err);
}
*/

#else

/*
//----------------------------------------------------------------------------
void
ICACHE_FLASH_ATTR
znet_debug_log_error(const  ZWError err, const char* func)
{
    //if( err != NoError ) znet_log(znet, Error, "Error returned from %s: %s
(%d)\r\n", func, znet_strerror(err), err);
}
*/

#endif
