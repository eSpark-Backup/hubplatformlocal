#include <iostream>
#include <typeinfo>

#include "json_utils/json_utils.h"
#include "log_utils/ezlo_log.h"
#include <tiny/tiny_consolecolors.h>

#include "logcmd/log_arg_parse.h"
#include "logcmd/log_command.h"
#include "logcmd/log_exceptions.h"

using namespace HubPlatform::LogCmd;

namespace {

Args getInputArgs( int argc, char* argv[] ) {
    Args args;
    for( int i = 1; i < argc; ++i )
        args.emplace_back( argv[ i ] );
    return args;
}

}  // namespace

int main( int argc, char* argv[] ) {
    try {
        auto args = parseArgs( getInputArgs( argc, argv ) );

        LogCommand logCommand;
        return logCommand.run( args );
    }
    catch( LogArgError& e ) {
        LOGGER_ERROR_RAW( cc::error( "Arguments error: " ) + cc::note( e.what() ) );
        help();
    }
    catch( std::exception& e ) {
        LOGGER_ERROR_RAW( cc::error( "Exception (" + std::string( typeid( e ).name() ) + "): " ) +
                          cc::note( e.what() ) );
    }
    catch( ... ) {
        LOGGER_ERROR_RAW( cc::error( "Unknown exception was caught" ) );
    }
    return -1;
}
