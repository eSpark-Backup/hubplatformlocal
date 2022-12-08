#include "cored/model/cored_feature.h"

#include "json_utils/json_utils.h"

namespace HubPlatform {
namespace CoreD {
namespace Model {

Feature::Feature( Json json )
: m_json( std::move( json ) ) {}

std::string Feature::getName() const {
    return json_get_str( m_json, "_id" );
}

bool Feature::getStatus() const {
    return json_get_bool( m_json, "status", false );
}

Feature::Version Feature::getVersion() const {
    return Feature::Version( json_get_str( m_json, "version" ) );
}

bool Feature::isEditable() const {
    return json_get_bool( m_json, "editable", false );
}

bool Feature::setStatus( bool status ) {
    if( !isEditable() )
        return false;

    m_json[ "status" ] = status;
    return true;
}

}  // namespace Model
}  // namespace CoreD
}  // namespace HubPlatform
