#pragma once

#include "database_client/idbclient.h"

namespace HubPlatform {
namespace CoreD {

class IFeaturesManager {
public:
    virtual ~IFeaturesManager();

    virtual void setFeatureStatus( const std::string& feature, bool status ) = 0;
    virtual bool getFeatureStatus( const std::string& feature ) const = 0;
};

class FeaturesManager final : public IFeaturesManager {
public:
    explicit FeaturesManager( IDbClient& dbClient );

    void setFeatureStatus( const std::string& feature, bool status ) override;
    bool getFeatureStatus( const std::string& feature ) const override;

private:
    IDbClient& m_dbClient;
};

}  // namespace CoreD
}  // namespace HubPlatform
