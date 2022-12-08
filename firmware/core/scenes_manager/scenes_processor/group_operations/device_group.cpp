#include "device_group.h"
#include "scenes_manager/scenes_engine/model/model_resources.h"
#include <utility>

namespace HubPlatform {
namespace ScenesManager {
using namespace SceneEngine::Model::Resources;

DeviceGroupInfo::DeviceGroupInfo( std::string id,
                                  std::string name,
                                  JsonArray categories,
                                  JsonArray devices,
                                  JsonArray exceptions,
                                  bool persistent )
: m_id( std::move( id ) )
, m_name( std::move( name ) )
, m_categories( std::move( categories ) )
, m_devices( std::move( devices ) )
, m_exceptions( std::move( exceptions ) )
, m_persistent( persistent ) {}

void DeviceGroupInfo::setId( std::string id ) {
    m_id = std::move( id );
}

const std::string& DeviceGroupInfo::getId() const {
    return m_id;
}

const std::string& DeviceGroupInfo::getName() const {
    return m_name;
}

const JsonArray& DeviceGroupInfo::getCategories() const {
    return m_categories;
}

const JsonArray& DeviceGroupInfo::getDevices() const {
    return m_devices;
}

void DeviceGroupInfo::setDevices( const JsonArray& devices ) {
    m_devices = std::move( devices );
}

const JsonArray& DeviceGroupInfo::getExceptions() const {
    return m_exceptions;
}

void DeviceGroupInfo::setExceptions( const JsonArray& exceptions ) {
    m_exceptions = std::move( exceptions );
}

bool DeviceGroupInfo::getPersistent() const {
    return m_persistent;
}

Json DeviceGroupInfo::toJson() const {
    JsonObject result = JSON_EMPTY_OBJ;
    result[ "_id" ] = m_id;
    result[ DeviceGroup::Name ] = m_name;
    result[ DeviceGroup::Categories ] = m_categories;
    result[ DeviceGroup::Devices ] = m_devices;
    result[ DeviceGroup::Exceptions ] = m_exceptions;
    result[ DeviceGroup::Persistent ] = m_persistent;

    return result;
}

bool DeviceGroupInfo::fromJson( const JsonObject& data ) {
    std::string status;
    m_id = json_get_str( data, "_id" );
    m_name = json_get_str( data, DeviceGroup::Name );
    json_get_arr( data, DeviceGroup::Categories, m_categories );
    json_get_arr( data, DeviceGroup::Devices, m_devices );
    json_get_arr( data, DeviceGroup::Exceptions, m_exceptions );
    m_persistent = json_get_bool( data, DeviceGroup::Persistent, false );

    return true;
}

}  // namespace ScenesManager
}  // namespace HubPlatform
