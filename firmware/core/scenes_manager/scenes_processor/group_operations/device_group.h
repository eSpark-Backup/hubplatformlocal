#pragma once

#include <string>
#include "json_utils/json_utils.h"
#include "log_utils/ezlo_log.h"
#include "group.h"

namespace HubPlatform {
namespace ScenesManager {

class DeviceGroupInfo : public GroupInfo {
public:
    DeviceGroupInfo() = default;
    DeviceGroupInfo( std::string id,
                     std::string name,
                     JsonArray categories,
                     JsonArray devices,
                     JsonArray exceptions,
                     bool persistent );

public:
    void setId( std::string id );
    const std::string& getId() const;
    const std::string& getName() const;
    const JsonArray& getCategories() const;
    const JsonArray& getDevices() const;
    void setDevices( const JsonArray& devices );
    const JsonArray& getExceptions() const;
    void setExceptions( const JsonArray& exceptions );
    bool getPersistent() const;

public:
    Json toJson() const override;
    bool fromJson( const JsonObject& data ) override;

private:
    std::string m_id;
    std::string m_name;
    JsonArray m_categories;
    JsonArray m_devices;
    JsonArray m_exceptions;
    bool m_persistent{ false };
};

}  // namespace ScenesManager
}  // namespace HubPlatform