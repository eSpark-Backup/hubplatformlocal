#include "scenes_manager/scenes_engine/events/device_state_event.h"
#include "scenes_manager/scenes_engine/model/model_resources.h"
#include "database_client/dbqueryparser.h"
#include "json_utils/json_utils.h"
#include "log_utils/ezlo_log.h"

namespace SceneEngine {

DeviceStateEvent::DeviceStateEvent( const DeviceStateParams& params )
: m_params( params ) {}

std::string DeviceStateEvent::getMethodName() const {
    return Model::Resources::MethodNames::isDeviceState;
}

bool DeviceStateEvent::checkDeviceInfo( const JsonObject& message ) {
    auto id = json_get_str( message, "_id" );
    auto cachedId = json_get_str( m_cachedDeviceInfo, "id" );
    if( !cachedId.empty() && cachedId != id ) {
        m_cachedDeviceInfo = JSON_EMPTY_OBJ;
    }
    else {
        m_cachedDeviceInfo[ "id" ] = id;
    }

    for( auto it = message.begin(); it != message.end(); ++it ) {
        const auto& key = it.key();
        m_cachedDeviceInfo[ key ] = it.value();
    }

    DeviceStateParams::iterator it = m_params.begin();
    while( it != m_params.end() ) {
        auto key = it->first;
        auto value = it->second;
        JsonObject deviceValue;

        json_get( m_cachedDeviceInfo, key, deviceValue );
        if( value != deviceValue ) {
            return false;
        }

        ++it;
    }

    return true;
}

bool DeviceStateEvent::compare( const JsonObject& deviceInfo ) {
    bool compareResult = checkDeviceInfo( deviceInfo );

    if( m_keepState ) {
        if( !m_triggered && compareResult ) {
            m_triggered = true;
            return true;
        }

        if( !compareResult ) {
            m_triggered = false;
        }

        return false;
    }

    return compareResult;
}

void DeviceStateEvent::setKeepState( bool value ) {
    m_keepState = value;
}

}  // namespace SceneEngine
