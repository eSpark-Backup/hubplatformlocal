#include "vidoo4hub/vidoo4hub_exception.h"

#include <typeinfo>

namespace HubPlatform {
namespace Vidoo4Hub {

std::string whatError( const std::exception& e ) {
    return "( " + std::string( typeid( e ).name() ) + " ): " + e.what();
};

}  // namespace Vidoo4Hub
}  // namespace HubPlatform
